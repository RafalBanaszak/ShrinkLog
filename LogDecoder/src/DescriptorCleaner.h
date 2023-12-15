//
// Created by rafal on 15.12.2023.
//

#ifndef SHRINKLOG_DESCRIPTORCLEANER_H
#define SHRINKLOG_DESCRIPTORCLEANER_H

#include "MessageDescriptorStorage.h"

namespace sl {
    struct DescriptorCleaner {
        static void CleanDescriptors(std::unique_ptr<MessageDescriptorStorage>&) noexcept;
    };

} // sl

#endif //SHRINKLOG_DESCRIPTORCLEANER_H
