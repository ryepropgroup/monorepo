#pragma once
#include <string>
#include <memory>
#include <unordered_map>
#include <atomic>
#include "mach/sequences/sequence.hpp"

namespace mach {

/**
 * @brief Factory class for creating actions.
 */
class SequenceManager {
    public:
        static SequenceManager& getInstance() {
            static SequenceManager instance;
            return instance;
        }

        SequenceManager(SequenceManager const&) = delete;
        void operator=(SequenceManager const&) = delete;

        /**
         * @brief Registers a sequence to be executed.
         * 
         * @param sequenceName The name of the sequence.
         * @param sequence A unique pointer to the sequence.
         */
        void addSequence(const std::string& sequenceName, std::shared_ptr<Sequence> sequence) {
            sequences[sequenceName] = sequence;
        }

        std::vector<std::string> getSequenceNames() {
            std::vector<std::string> sequenceNames;
            for (const auto& sequence : sequences) {
                sequenceNames.push_back(sequence.first);
            }
            return sequenceNames;
        }

        std::shared_ptr<Sequence> getSequence(const std::string& sequenceName) {
            return sequences[sequenceName];
        }
    
    private:
        SequenceManager() {}

        std::unordered_map<std::string, std::shared_ptr<Sequence>> sequences;
        std::atomic<bool> playingAbortSequence = false;
    };
}