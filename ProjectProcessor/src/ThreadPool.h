// MIT License
//
// Copyright (c) 2024 ShrinkLog Contributors
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

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
    std::mutex printMutex;
    std::condition_variable condVar;
    std::atomic<bool> terminate;
    std::atomic<bool> queueNonEmpty;
    std::atomic<unsigned long long> errorCnt;
    std::atomic<unsigned> exitCnt;

    template<class Function, class... Args>
    void WorkerWrapper(Function fn, Args... args) {
        using namespace std::chrono_literals;

        while (!terminate.load() || queueNonEmpty.load()) {
            std::unique_lock lock(cvMutex);
            condVar.wait(lock, [&rQueueNonEmpty = queueNonEmpty, &rTerminate = terminate] {
                return (rQueueNonEmpty.load() || rTerminate.load());
            });
            lock.unlock();

            // process data as long as something is in queue
            do {
                auto job = GetJob();
                if(!job) {
                    continue;
                }

                if (queueNonEmpty.load()) { condVar.notify_one(); }
                try {
                    fn(job.value(), args...);
                } catch (const std::exception& err) {
                    std::lock_guard printLockGuard{printMutex};
                    fmt::print(stderr, "An exception occurred in a thread pool worker function provided by the user\n"
                                       "The thread will NOT be terminated.\n"
                                       "{}\n", err.what());
                    errorCnt.fetch_add(1, std::memory_order_relaxed);
                } catch ( ... ) {
                    fmt::print(stderr, "Unknown exception in worker thread\n"
                                       "The thread will NOT be terminated.\n");
                    errorCnt.fetch_add(1, std::memory_order_relaxed);
                }
            } while (queueNonEmpty.load());
        }
        exitCnt.fetch_add(1, std::memory_order_relaxed);
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

public:
    template<class Function, class... Args>
    ThreadPool(const uint8_t threadCnt, Function fn, Args... args) noexcept : terminate{false}, queueNonEmpty{true}, exitCnt{0} {
        pool.reserve(threadCnt);
        for (unsigned i = 0; i < threadCnt; ++i) {
            pool.emplace_back(&ThreadPool::WorkerWrapper<Function, Args...>, this, fn, args...);
        }
    }

    ~ThreadPool() {
        if (!terminate.load()) {
            JoinAll();
        }
    }

    void AddJob(const JobType& job) noexcept {
        std::lock_guard lock{qMutex};
        queueNonEmpty.store(true);
        queue.emplace(job);
        condVar.notify_one();
    }

    void AddJob(JobType&& job) noexcept {
        std::lock_guard lock{qMutex};
        queueNonEmpty.store(true);
        queue.emplace(std::move(job));
        condVar.notify_one();
    }

    void JoinAll() noexcept {
        terminate.store(true);

        while(exitCnt.load() < pool.size()) {
            condVar.notify_all();
            std::this_thread::yield();
        }

        for (auto& thr : pool) {
            try {
                thr.join();
            } catch ( ... ) {
                // if a thread finish execution before join just ignore it
            }
        }
    }

    [[nodiscard]] unsigned long long GetErrors() const noexcept {
        return errorCnt.load();
    };
};


#endif //SHRINKLOG_THREADPOOL_H
