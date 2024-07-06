#pragma once
#include <string>

namespace mach {

    /**
     * @brief Parses all sequences'config/sequences/' and registers them in SequenceManager.
     */
    void parseAllSequences(const std::string sequencesFolder);
}