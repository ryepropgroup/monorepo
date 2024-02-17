#include <yaml-cpp/yaml.h>

#define CONFIG_FILE "config.yml"

namespace ConfigParser {
}

void parseConfig() {
    YAML::Node config = YAML::LoadFile(CONFIG_FILE);
    
    YAML::Node valves = config["valves"];
    for (YAML::const_iterator it = valves.begin(); it != valves.end(); it++) {
        std::string name = it->first.as<std::string>();
        std::string pin = it->second.as<std::string>();
    }
    
}