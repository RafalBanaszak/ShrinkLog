//
// Created by rbanasza on 18.09.2023.
//

#include "IntfMasterMap.h"

#include <iostream>

namespace sl {

    [[nodiscard]] IntfMasterMap::OutputStatus IntfMasterMap::Modify(
            std::vector<std::string>& append,
            std::vector<std::string>& remove) noexcept
    {
        (void)append;
        (void)remove;
        std::cerr
            << "ERROR: "                                    \
            << __FILE__ <<":" << __FUNCTION__               \
            << " This is a pure virtual function."          \
               " It should be never called directly! :("    \
            << std::endl;
            return OutputStatus::ImplementationError;
    }

} // sl