#pragma once
#include <string>
#include <memory>
#include <unordered_map>
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
        void addSequence(const std::string& sequenceName, std::unique_ptr<Sequence> sequence) {
            sequences[sequenceName] = std::move(sequence);
        }

        /**
         * @brief Executes a sequence.
         * 
         * @param sequenceName The name of the sequence to execute.
         */
        void executeSequence(const std::string& sequenceName) {
            if (sequences.find(sequenceName) == sequences.end()) {
                spdlog::warn("Sequence '{}' not found, ignoring!", sequenceName);
                return;
            }
            sequences[sequenceName]->execute();
        }

        std::vector<std::string> getSequenceNames() {
            std::vector<std::string> sequenceNames;
            for (const auto& sequence : sequences) {
                sequenceNames.push_back(sequence.first);
            }
            return sequenceNames;
        }
    
    private:
        SequenceManager() {}

        std::unordered_map<std::string, std::unique_ptr<Sequence>> sequences;
    };
}