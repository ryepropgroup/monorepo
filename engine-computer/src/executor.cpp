#include "mach/executor.hpp"
#include <thread>
#include <memory>
#include <spdlog/spdlog.h>
#include "mach/sequences/sequence_manager.hpp"
#include "mach/device/device_manager.hpp"

void mach::Executor::executeSequence(const std::string sequenceName) {
    std::shared_ptr<Sequence> sequence = SequenceManager::getInstance().getSequence(sequenceName);
    if (sequence == nullptr) {
        spdlog::warn("Sequence '{}' not found, skipping execution!", sequenceName);
        return;
    }

    if (executing.exchange(true, std::memory_order_acq_rel)) {
        if (!sequence->isOverride()) {
            spdlog::warn("MACH: Skipped executing sequence {}, another sequence is already being executed.", sequenceName);
            return;
        }
        DeviceManager::getInstance().enableAbortMode();
    }

    std::thread([this, sequence]() {
        spdlog::info("MACH: Executing sequence {}.", sequence->getName());
        sequence->execute();
        spdlog::info("MACH: Finished sequence {}.", sequence->getName());
        // If this sequence was an abort, disable the abort mode after 5 seconds.
        if (sequence->isOverride()) {
            std::this_thread::sleep_for(std::chrono::seconds(5));
            DeviceManager::getInstance().disableAbortMode();
        }
        this->executing.store(false, std::memory_order_release);
    }).detach();
}
