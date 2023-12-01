//
// Created by rafal on 25.11.2023.
//

#include "Decoder.h"

#include "fmt/core.h"

namespace dc {

    Decoder::Decoder(std::unique_ptr<MessageDescriptorStorage> &&messageMap) noexcept {
        fmt::print("Cache loaded! {}\n", messageMap->GetDescriptor(2).msgChunks.size());
    }
} // dc