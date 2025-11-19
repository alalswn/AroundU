#include "recommendation_engine.h"

#include <algorithm> // std::sort
#include <cctype>    // std::tolower
#include <string>
#include <utility>   // std::pair

// ---- 작은 유틸: 문자열 소문자 변환 ----
namespace {
    std::string toLowerCopy(const std::string& s) {
        std::string out;
        out.reserve(s.size());
        for (char ch : s) {
            out.push_back(static_cast<char>(std::tolower(static_cast<unsigned char>(ch))));
        }
        return out;
    }

    bool containsIgnoreCase(const std::string& text, const std::string& pattern) {
        std::string t = toLowerCopy(text);
        std::string p = toLowerCopy(pattern);
        return t.find(p) != std::string::npos;
    }
}

// ---- 생성자 ----
RecommendationEngine::RecommendationEngine(EventRepository& repo)
    : repository(repo) {}

// ---- 위치 기반 추천 ----
std::vector<CulturalEvent>
RecommendationEngine::recommendByLocation(const GeoLocation& userLocation, double radiusKm)
{
    std::vector<CulturalEvent> all = repository.findAll();

    // (거리, 이벤트) 쌍을 임시로 모아두기
    std::vector<std::pair<double, CulturalEvent>> filtered;

    filtered.reserve(all.size());

    for (const auto& ev : all) {
        // GeoLocation::distanceTo 사용
        double d = userLocation.distanceTo(ev.location);

        // radiusKm 안에 들어오는 이벤트만 선택
        if (d <= radiusKm) {
            filtered.emplace_back(d, ev);
        }
    }

    // 거리 오름차순으로 정렬
    std::sort(filtered.begin(), filtered.end(),
              [](const auto& a, const auto& b) {
                  return a.first < b.first; // 거리 비교
              });

    // CulturalEvent만 뽑아서 결과 벡터 만들기
    std::vector<CulturalEvent> result;
    result.reserve(filtered.size());
    for (const auto& pair : filtered) {
        result.push_back(pair.second); // 복사
    }

    return result;
}

