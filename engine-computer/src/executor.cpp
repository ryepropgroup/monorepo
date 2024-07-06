#include "mach/executor.hpp"
#include <thread>
#include <spdlog/spdlog.h>
#include "mach/sequences/sequence_manager.hpp"

static void executeSequenceThread(const std::string sequenceName, std::atomic_bool& executing);

void mach::Executor::executeSequence(const std::string sequenceName) {
    if (executing.exchange(true, std::memory_order_acq_rel)) {
        spdlog::warn("MACH: Skipped executing sequence {}, another sequence is already being executed.", sequenceName);
        return;
    }
    std::thread(executeSequenceThread, sequenceName, std::ref(executing)).detach();
}

static void executeSequenceThread(const std::string sequenceName, std::atomic_bool& executing) {
    spdlog::info("MACH: Executing sequence {}.", sequenceName);
    mach::SequenceManager::getInstance().executeSequence(sequenceName);
    spdlog::info("MACH: Finished sequence {}.", sequenceName);
    executing.store(false, std::memory_order_release);
}
