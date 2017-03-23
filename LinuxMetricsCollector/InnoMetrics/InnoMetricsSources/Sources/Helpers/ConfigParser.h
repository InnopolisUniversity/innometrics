//
// Created by alon on 10.03.17.
//

#ifndef INNOMETRICS_CONFIGPARSER_H
#define INNOMETRICS_CONFIGPARSER_H
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

struct Config {
    std::string    server_url;
    std::string    update_url;
    bool           manual;
    int            hours;
};

void loadConfig(Config& config) {
    std::ifstream fin("inno.cfg");
    std::string line;
    while (std::getline(fin, line)) {
        std::istringstream sin(line.substr(line.find("=") + 1));
        if (line.find("server_url") != -1)
            sin >> config.server_url;
        else if (line.find("update_url") != -1)
                sin >> config.update_url;
        else if (line.find("manual") != -1)
            sin >> config.manual;
        else if (line.find("hours") != -1)
            sin >> config.hours;
    }
}

void saveConfig(Config& config) {
    std::ofstream fout("inno.cfg");
    std::string line;
    fout << "server_url=" << config.server_url << "\n";
    fout << "update_url=" << config.update_url << "\n";
    fout << "manual=" << config.manual << "\n";
    fout << "hours=" << config.hours << "\n";
}
#endif //INNOMETRICS_CONFIGPARSER_H
