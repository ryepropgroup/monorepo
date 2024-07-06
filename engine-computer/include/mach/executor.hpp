#pragma once
#include <atomic>
#include <string>
#include <condition_variable>

namespace mach {

/**
 * @brief Class for executing sequences.
 */
class Executor {
    public:
        static Executor& getInstance() {
            static Executor instance;
            return instance;
        }

        Executor(Executor const&) = delete;
        void operator=(Executor const&) = delete;

        /**
         * @brief Executes a sequence.
         * 
         * @param sequenceName The name of the sequence to execute.
         */
        void executeSequence(const std::string sequenceName);

    
    private:
        Executor() {}

        std::atomic_bool executing = false;
        std::atomic_bool abort = false;
        std::condition_variable abortCV;
    };
}