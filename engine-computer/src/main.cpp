#include "mach/main.hpp"
#include <iostream>
#include <spdlog/spdlog.h>
#include <yaml-cpp/yaml.h>
#include "mach/labjack.hpp"
#include "mach/config_parser.hpp"
#include "mach/device/device_manager.hpp"
#include "mach/sequences/action_factory.hpp"
#include "mach/sequences/open_action.hpp"
#include "mach/device/device.hpp"
#include "mach/sequences/sleep_action.hpp"
#include "mach/sequences/close_action.hpp"
#include "mach/sequences/sequence_parser.hpp"
#include "mach/sequences/check_action.hpp"
#include "mach/grpc_client.hpp"
#include "mach/gui.hpp"
#include "absl/flags/flag.h"
#include "absl/flags/parse.h"

#define LABJACK_CONFIG std::string("config/labjacks/")
#define REMOTE_CONFIG std::string("config/remote.yml")
#define SEQUENCES_CONFIG std::string("config/sequences/")

ABSL_FLAG(bool, demo, false, "Demo mode");

static bool demo = false;

int main(int argc, char* argv[]) {
    spdlog::info("MACH: Hello World!");
    absl::ParseCommandLine(argc, argv);

    demo = absl::GetFlag(FLAGS_demo);
    spdlog::info("MACH: Demo mode: {}", demo);

    // Read configuration 🤓
    mach::parseAllLabjacks(LABJACK_CONFIG);
    mach::parseRemoteConfig(REMOTE_CONFIG);

    // Print all devices for debug.
    mach::DeviceManager& deviceManager = mach::DeviceManager::getInstance();
    // deviceManager.printDevices();

    // Register sequence actions.
    mach::ActionFactory& actionFactory = mach::ActionFactory::getInstance();
    actionFactory.registerAction("OPEN", []() { return std::make_unique<mach::OpenAction>(); });
    actionFactory.registerAction("CLOSE", []() { return std::make_unique<mach::CloseAction>(); });
    actionFactory.registerAction("SLEEP", []() { return std::make_unique<mach::SleepAction>(); });
    actionFactory.registerAction("CHECK", []() { return std::make_unique<mach::CheckAction>(); });

    // Parse all sequences.
    mach::parseAllSequences(SEQUENCES_CONFIG);

    // Start streaming all LabJacks.
    // deviceManager.connectAllLabjacks();
    deviceManager.startAllLabjackStreams();

    // Start gRPC client.
    mach::grpcStartClient(argc, argv);

    // Start GUI.
    // mach::gui::startGUI(argc, argv);

    spdlog::info("MACH: Reached end of main."); //🧏‍♂️🤫
    while (true) {}
}

bool mach::isDemo() {
    return demo;
}
