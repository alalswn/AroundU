#pragma once
#include <string>
#include <httplib.h>

class ApiHandler {
private:
    std::string baseUrl;
    std::string apiKey;
    httplib::Client client;

public:
    ApiHandler(const std::string& url, const std::string& key);

    // start ~ end 범위로 API 호출하여 JSON 문자열 반환
    std::string fetchEvents(int startIndex, int endIndex);
};
