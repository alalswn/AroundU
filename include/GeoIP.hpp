#pragma once
#include <string>
#include <optional>
#include <regex>
#include <cstdio>        // _popen / popen
#include "GeoLocation.hpp"

class GeoIP {
public:
    static std::optional<GeoLocation> getUserLocation() {
        std::string body = runCurl("https://ipinfo.io/json");
        if (body.empty()) body = runCurl("https://ipapi.co/json");
        if (body.empty()) return std::nullopt;

        // 1) ipinfo.io: "loc":"37.5665,126.9780"
        {
            // 커스텀 구분자 LOC 사용: R"LOC( ... )LOC"
            std::regex re_loc(R"LOC("loc"\s*:\s*"([\-0-9\.]+),([\-0-9\.]+)")LOC");
            std::smatch m;
            if (std::regex_search(body, m, re_loc) && m.size() == 3) {
                double lat = std::stod(m[1].str());
                double lon = std::stod(m[2].str());
                return GeoLocation(lat, lon);
            }
        }
        // 2) ipapi.co: "latitude": 37.56, "longitude": 126.97
        {
            std::regex re_lat(R"LOC("latitude"\s*:\s*([\-0-9\.]+))LOC");
            std::regex re_lon(R"LOC("longitude"\s*:\s*([\-0-9\.]+))LOC");
            std::smatch m1, m2;
            if (std::regex_search(body, m1, re_lat) && std::regex_search(body, m2, re_lon)) {
                double lat = std::stod(m1[1].str());
                double lon = std::stod(m2[1].str());
                return GeoLocation(lat, lon);
            }
        }
        return std::nullopt;
    }

private:
    static std::string runCurl(const char* url) {
        // 1) wget 시도
        std::string cmd = std::string("wget -qO - ") + url;
#ifdef _WIN32
        FILE* pipe = _popen(cmd.c_str(), "r");
#else
        FILE* pipe = popen(cmd.c_str(), "r");
#endif
        if (!pipe) return {};

        char buf[4096]; std::string out;
        while (size_t n = fread(buf, 1, sizeof(buf), pipe)) out.append(buf, n);
#ifdef _WIN32
        _pclose(pipe);
#else
        pclose(pipe);
#endif

        // wget이 없거나 실패하면 curl로 재시도
        if (out.empty()) {
            cmd = std::string("curl -s ") + url;
#ifdef _WIN32
            pipe = _popen(cmd.c_str(), "r");
#else
            pipe = popen(cmd.c_str(), "r");
#endif
            if (!pipe) return {};
            while (size_t n = fread(buf, 1, sizeof(buf), pipe)) out.append(buf, n);
#ifdef _WIN32
            _pclose(pipe);
#else
            pclose(pipe);
#endif
        }
        return out;
    }
};