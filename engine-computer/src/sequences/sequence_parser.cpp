#include "mach/sequences/sequence_parser.hpp"
#include <memory>
#include <filesystem>
#include <yaml-cpp/yaml.h>
#include <spdlog/spdlog.h>
#include "mach/sequences/action_factory.hpp"
#include "mach/sequences/sequence.hpp"
#include "mach/sequences/sequence_manager.hpp"

namespace mach {

static void parseSequence(std::string file);

void parseAllSequences(const std::string sequencesFolder) {
    if (!std::filesystem::exists(sequencesFolder)) {
        spdlog::error("MACH: Sequences folder not found at '{}'!", sequencesFolder);
        throw std::runtime_error("Sequences folder file not found!");
    }
    spdlog::info("MACH: Reading sequences from folder '{}'.", sequencesFolder);

    std::filesystem::directory_iterator end_itr;
    for (std::filesystem::directory_iterator itr(sequencesFolder); itr != end_itr; ++itr) {
        if (std::filesystem::is_regular_file(itr->path())) {
            std::string file = itr->path().string();
            if (file.ends_with(".yml")) {
                parseSequence(file);
            }
        }
    }
}

static void parseSequence(std::string file) {
    YAML::Node config = YAML::LoadFile(file);

    std::string sequenceName = file.substr(file.find_last_of("/\\") + 1);
    sequenceName = sequenceName.substr(0, sequenceName.find_last_of("."));
    std::shared_ptr<Sequence> sequence = std::make_shared<Sequence>(sequenceName);
    spdlog::info("MACH: Parsing sequence from file '{}.yml'", sequenceName);

    ActionFactory& actionFactory = ActionFactory::getInstance();

    YAML::Node override = config["override"];
    if (override) {
        sequence->setOverride(override.as<bool>());
    }
    
    YAML::Node steps = config["steps"];
    for (YAML::const_iterator it = steps.begin(); it != steps.end(); it++) {
        const YAML::Node actionNode = *it;
        std::unique_ptr<Action> action = actionFactory.createAction(actionNode);
        if (!action) {
            YAML::Node invalid(actionNode);
            spdlog::error("MACH: Invalid action defined in sequence file '{}' (see below)!\n{}", sequenceName, YAML::Dump(invalid));
            throw std::runtime_error("Invalid action defined in sequence!");
        }
        sequence->addAction(std::move(action));
    }

    SequenceManager& sequenceManager = SequenceManager::getInstance();
    sequenceManager.addSequence(sequenceName, sequence);

    spdlog::info("MACH: Registered sequence: {}", sequenceName);
}

} // namespace mach