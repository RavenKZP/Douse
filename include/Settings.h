#pragma once

#include "REX/REX.h"

class Settings : public REX::Singleton<Settings> {
public:
    Settings() {
        std::ifstream file("Data\\SKSE\\Plugins\\Douse.ini");
        if (!file.is_open()) {
            return;
        }

        std::string line;
        while (std::getline(file, line)) {
            if (line.contains("fSubmerged")) {
                auto pos = line.find('=');
                if (pos != std::string::npos) {
                    std::string value = line.substr(pos + 1);
                    Submerged = std::stof(value);
                }
            }
        }
    }

    // Percentage of the actor's height that must be submerged in water to trigger the kill effect
    float Submerged = 0.5f;
};