//
// Created by rafal on 15.12.2023.
//

#include "DescriptorCleaner.h"

#include "fmt/core.h"

namespace sl {

    void DescriptorCleaner::CleanDescriptors(std::unique_ptr<MessageDescriptorStorage> &mds_p) noexcept {
        if (mds_p == nullptr) {
            return;
        }

        for (auto &descriptor: mds_p->_cache) {
            for (auto &chunk: descriptor.msgChunks) {
                std::string::size_type pos = 0;
                try {
                    while ((pos = chunk.message.find("\\n", pos)) != std::string::npos) {
                        chunk.message.replace(pos, 2, "\n");
                        pos += 1;
                    }
                    pos = 0;
                    while ((pos = chunk.message.find("\\t", pos)) != std::string::npos) {
                        chunk.message.replace(pos, 2, "\t");
                        pos += 1;
                    }
                } catch (std::exception &e) {
                    fmt::print(stderr, "Unable to clean following message chunk:{}\nEXCEPTION:{}", chunk.message,
                               e.what());
                }
            }
        }
    }
} //sl


