#include "mach/executor.hpp"
#include <thread>
#include <memory>
#include <spdlog/spdlog.h>
#include "mach/sequences/sequence_manager.hpp"

void mach::Executor::executeSequence(const std::string sequenceName) {
    std::shared_ptr<Sequence> sequence = SequenceManager::getInstance().getSequence(sequenceName);
    if (sequence == nullptr) {
        spdlog::warn("Sequence '{}' not found, skipping execution!", sequenceName);
        return;
    }

    if (!sequence->isOverride() && executing.exchange(true, std::memory_order_acq_rel)) {
        spdlog::warn("MACH: Skipped executing sequence {}, another sequence is already being executed.", sequenceName);
        return;
    }

    std::thread([this, sequence]() {
        spdlog::info("MACH: Executing sequence {}.", sequence->getName());
        sequence->execute();
        spdlog::info("MACH: Finished sequence {}.", sequence->getName());
        this->executing.store(false, std::memory_order_release);
    }).detach();
}
