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

// ---- 위치 기반 추천 (RecommendedEvent로 반환) ----
std::vector<RecommendedEvent>
RecommendationEngine::recommendByLocation(const GeoLocation& userLocation, double radiusKm)
{
    // 저장소에서 모든 이벤트 가져오기
    std::vector<CulturalEvent> all = repository.findAll();

    // (거리, 이벤트) 쌍을 임시로 보관
    std::vector<std::pair<double, CulturalEvent>> filtered;
    filtered.reserve(all.size());

    for (const auto& ev : all) {
        // GeoLocation::distanceTo 사용 (단위는 Km라고 가정)
        double d = userLocation.distanceTo(ev.location);

        // radiusKm 안에 들어오는 이벤트만 선택
        if (d <= radiusKm) {
            filtered.emplace_back(d, ev);
        }
    }

    // 거리 오름차순으로 정렬
    std::sort(filtered.begin(), filtered.end(),
              [](const auto& a, const auto& b) {
                  return a.first < b.first; // 거리 기준 비교
              });

    // RecommendedEvent 벡터로 변환
    std::vector<RecommendedEvent> result;
    result.reserve(filtered.size());
    for (const auto& pair : filtered) {
        RecommendedEvent re;
        re.event = pair.second;
        re.distanceKm = pair.first;
        result.push_back(re);
    }

    return result;
}

// ---- 행정구/장소 문자열 기반 추천 (간단 버전) ----
std::vector<CulturalEvent>
RecommendationEngine::recommendByDistrict(const std::string& districtName)
{
    std::vector<CulturalEvent> all = repository.findAll();
    std::vector<CulturalEvent> result;
    result.reserve(all.size());

    for (const auto& ev : all) {
        // CulturalEvent 안에 place(혹은 address) 같은 문자열 필드가 있다고 가정하고 사용
        // 필요한 경우 ev.place 대신 네가 실제로 쓰는 필드명으로 수정해줘!
        if (containsIgnoreCase(ev.place, districtName)) {
            result.push_back(ev);
        }
    }

    return result;
}
