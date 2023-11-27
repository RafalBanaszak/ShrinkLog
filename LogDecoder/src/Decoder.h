//
// Created by rafal on 25.11.2023.
//

#ifndef SHRINKLOG_DECODER_H
#define SHRINKLOG_DECODER_H

#include "MessageDescriptor.h"
#include "File.h"

#include <filesystem>
#include <regex>
#include <sstream>
#include <map>
#include <shared_mutex>

namespace dc {

    class Decoder {
        const static std::regex linePattern;
        const static std::regex argPattern;
        static const std::regex substringArgPattern;

        ptrdiff_t linesNumber{};
        std::vector<MessageDescriptor> descriptors;
        std::shared_mutex descMtx;

        void LoadMapLine(const std::string &line) noexcept;
        static std::vector<MessageDescriptor::Chunk>
        DivideMessageToChunks(const std::string &arguments, std::string message) ;

    public:
        //TODO: Experiment, refactor
        void LoadMap(const std::filesystem::path &path);
        [[nodiscard]] std::string DecodeSingle(std::vector<char> message) noexcept;

    };

} // dc

#endif //SHRINKLOG_DECODER_H
