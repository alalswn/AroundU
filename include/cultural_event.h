#pragma once
#include <string>
#include "geo.h"
#include <nlohmann/json.hpp>

class CulturalEvent {
public:
    std::string id;
    std::string title;
    std::string place;
    std::string date;
    std::string codeName;
    std::string themeCode;
    std::string program;

    GeoLocation location;

    CulturalEvent() = default;

    static CulturalEvent fromJson(const nlohmann::json& j);
};

