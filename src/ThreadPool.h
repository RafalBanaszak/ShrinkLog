//
// Created by rafal on 25.10.2023.
//

#ifndef SHRINKLOG_THREADPOOL_H
#define SHRINKLOG_THREADPOOL_H

#include <chrono>
#include <condition_variable>
#include <functional>
#include <vector>
#include <thread>
#include <queue>
#include <optional>

#include <fmt/core.h>

template<typename JobType>
class ThreadPool {
    std::vector<std::thread> pool;
    std::queue<JobType> queue;
    std::mutex qMutex;
    std::mutex cvMutex;
    std::condition_variable condVar;
    std::atomic<bool> terminate;
    std::atomic<unsigned> exitCnt;
    std::atomic<bool> queueNonEmpty;

    std::mutex prMutex;

    template<class Function, class... Args>
    void WorkerWrapper(unsigned id, Function fn, Args... args) {
        using namespace std::chrono_literals;

        while (!terminate.load() || queueNonEmpty.load()) {
            std::unique_lock lock(cvMutex);
            condVar.wait(lock, [&queueNonEmpty = queueNonEmpty, &terminate = terminate] {
                return (queueNonEmpty.load() || terminate.load());
            });
            lock.unlock();

            // process data as long as something is in queue
            do {
                auto job = GetJob();
                if(!job) {
                    continue;
                }

                if (queueNonEmpty.load()) { condVar.notify_one(); }

                fn(job.value(), args...);
            } while (queueNonEmpty.load());
        }
        exitCnt.fetch_add(1, std::memory_order_relaxed);
    }


public:
    template<class Function, class... Args>
    ThreadPool(const unsigned threadCnt, Function fn, Args... args) noexcept : terminate{false}, queueNonEmpty{true}, exitCnt{0}  {
        pool.reserve(threadCnt);
        for (unsigned i = 0; i < threadCnt; ++i) {
            pool.emplace_back(&ThreadPool::WorkerWrapper<Function, Args...>, this, i, fn, args...);
        }
    }

    void AddJob(const JobType& job) noexcept {
        std::lock_guard lock{qMutex};
        queueNonEmpty.store(true);
        queue.emplace(job);
        condVar.notify_one();
    }

    std::optional<JobType> GetJob() noexcept {
        auto lock = std::unique_lock{qMutex, std::try_to_lock};

        if(!lock) {
            return std::nullopt;
        }

        if(queue.empty()) {
            queueNonEmpty.store(false);
            return std::nullopt;
        }

        auto tmp = queue.front();
        queue.pop();
        return tmp;
    }

    void FinishAndTerminate() {
        terminate.store(true);

        while(exitCnt.load() < pool.size()) {
            condVar.notify_all();
            std::this_thread::yield();
        }

        for (auto& thr : pool) {
            try {
                if (thr.joinable()) {
                    thr.join();
                }
            } catch ( const std::exception& e ) {
                // if a thread finish execution before join just ignore it
            }
        }
    }
};


#endif //SHRINKLOG_THREADPOOL_H
