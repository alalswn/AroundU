#pragma once
#include <string>
#include <optional>
#include <initializer_list>
#include <sstream>
#include <cctype>
#include "GeoLocation.hpp"
#include <json/json.h>

class CulturalEvent {
public:
    std::string id, title, place, date, codeName, themeCode, program;
    GeoLocation  location;

    static CulturalEvent fromJson(const Json::Value& j) {
        CulturalEvent ev;
        ev.id = pickString(j, { "id","ID","DOCID","SEQ","seq" });
        ev.title = pickString(j, { "title","TITLE","TITLE_NM","prfnm" });
        ev.place = pickString(j, { "place","PLACE","PLACE_NM","fcltynm","placenm" });
        ev.date = pickString(j, { "date","DATE","PERIOD","prfpdfromto","prfpd" });
        ev.codeName = pickString(j, { "codeName","CODENAME","codename","genre","GENRE" });
        ev.themeCode = pickString(j, { "themeCode","THEMECODE","themecode","theme","THEME" });
        ev.program = pickString(j, { "program","PROGRAM","content","CONTENTS","programName" });

        double lat = pickNumber(j, { "LAT","lat","latitude","Latitude","Y","y" });
        double lon = pickNumber(j, { "LOT","lon","longitude","Longitude","X","x","LON" });
        ev.location = GeoLocation(lat, lon);
        return ev;
    }

    static std::optional<CulturalEvent> tryFromJson(const Json::Value& j) noexcept {
        try { return fromJson(j); }
        catch (...) { return std::nullopt; }
    }

private:
    static std::string pickString(const Json::Value& j,
        std::initializer_list<const char*> keys) {
        for (auto k : keys) {
            if (!j.isMember(k) || j[k].isNull()) continue;
            const Json::Value& v = j[k];
            if (v.isString()) return v.asString();
            if (v.isBool())   return v.asBool() ? "true" : "false";
            if (v.isNumeric()) {
                std::ostringstream os;
                if (v.isIntegral()) os << v.asInt64();
                else                os << v.asDouble();
                return os.str();
            }
        }
        return {};
    }

    static double pickNumber(const Json::Value& j,
        std::initializer_list<const char*> keys) {
        for (auto k : keys) {
            if (!j.isMember(k) || j[k].isNull()) continue;
            const Json::Value& v = j[k];
            if (v.isNumeric()) return v.asDouble();
            if (v.isString()) {
                std::string s = trim(v.asString());
                if (!s.empty()) {
                    try { return std::stod(s); }
                    catch (...) {}
                }
            }
        }
        throw std::invalid_argument("latitude/longitude not found or invalid");
    }

    static std::string trim(std::string s) {
        auto issp = [](unsigned char c) { return std::isspace(c); };
        while (!s.empty() && issp(s.front())) s.erase(s.begin());
        while (!s.empty() && issp(s.back()))  s.pop_back();
        return s;
    }
};
