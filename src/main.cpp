#include <iostream>
#include <boost/asio.hpp>
#include <spdlog/spdlog.h>
#include <yaml-cpp/yaml.h>
#include "include/labjack.hpp"
#include "include/config_parser.hpp"
//
// Created by danie on 2023-12-14.
//

int main() {
    spdlog::info("Hello, {}!", "World");
    YAML::Emitter out;
    out << YAML::BeginSeq;
    out << "eggs";
    out << "bread";
    out << "milk";
    out << YAML::EndSeq;
    std::cout<< out.c_str()<<std::endl;


    // One LabJack for now...
    mach::LabJack labJack("LabJack");
    mach::parser::parseConfig(labJack);
    for (int i = 0; i < labJack.devices.size(); i++) {
        labJack.devices[i]->print();
    }
}