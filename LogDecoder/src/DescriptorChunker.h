//
// Created by rafal on 17.12.2023.
//

#ifndef SHRINKLOG_DESCRIPTORCHUNKER_H
#define SHRINKLOG_DESCRIPTORCHUNKER_H

#include "MessageDescriptor.h"

namespace sl::DescriptorChunker {
    [[nodiscard]] MessageDescriptor DivideToChunks(const std::string &arguments, std::string message);
} // sl::DescriptorChunker

#endif //SHRINKLOG_DESCRIPTORCHUNKER_H
