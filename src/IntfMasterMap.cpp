//
// Created by rbanasza on 18.09.2023.
//

#include "IntfMasterMap.h"

#include "fmt/core.h"

namespace sl {

    [[nodiscard]] IntfMasterMap::OutputStatus IntfMasterMap::Modify(
      std::vector<std::string> &append,
      std::vector<std::string> &remove) noexcept {
        (void) append;
        (void) remove;
        fmt::print(stderr, "ERROR: {}:{} Implementation error."
                           "This is a pure virtual function. It should be never called directly!\n",
                           __FILE__, __FUNCTION__);

        return OutputStatus::ImplementationError;
    }

} // sl