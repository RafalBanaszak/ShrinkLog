//
// Created by rbanasza on 18.09.2023.
//

#ifndef SHRINKLOG_INTFMASTERMAP_H
#define SHRINKLOG_INTFMASTERMAP_H

#include <string>
#include <vector>

namespace sl {

    struct IntfMasterMap {
        enum class OutputStatus{
            OK = 0,
            ImplementationError,
        };

        [[nodiscard]] virtual OutputStatus Modify(
                std::vector<std::string>& append,
                std::vector<std::string>& remove) noexcept = 0;
    };

} // sl

#endif //SHRINKLOG_INTFMASTERMAP_H
