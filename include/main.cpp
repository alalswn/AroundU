// main.cpp
#include <iostream>
#include <iomanip>
#include <vector>
#include <algorithm>
#include <memory>
#include "GeoLocation.hpp"
#include "GeoIP.hpp"
#include "CulturalEvent.hpp"   // JsonCpp 버전 (fromJson(const Json::Value&))

#include <json/json.h>         // JsonCpp

int main() {
    // 1) 샘플 응답(JSON) – API 주시면 json형태로 연결 예정 실제 API 대신
    const std::string jsonText = R"JSON(
    {
      "DATA": [
        {
          "ID": "E2025-001",
          "TITLE": "Seoul Jazz Night",
          "PLACE": "Seoul Plaza",
          "DATE": "2025-11-03 ~ 2025-11-05",
          "CODENAME": "공연",
          "THEMECODE": "TH01",
          "PROGRAM": "Live jazz by local artists",
          "LAT": 37.566535,
          "LOT": 126.977969
        },
        {
          "SEQ": "E2025-002",
          "TITLE_NM": "Cheonggyecheon Light Show",
          "PLACE_NM": "Cheonggyecheon",
          "PERIOD": "2025-11-10 ~ 2025-11-20",
          "codename": "전시",
          "themecode": "TH02",
          "content": "Night illumination event",
          "latitude": 37.5694,
          "longitude": 126.9779
        }
      ]
    }
    )JSON";

    // 2) JsonCpp 파싱
    Json::CharReaderBuilder rb;
    std::unique_ptr<Json::CharReader> reader(rb.newCharReader());
    Json::Value root;
    std::string errs;
    bool ok = reader->parse(jsonText.data(), jsonText.data() + jsonText.size(), &root, &errs);
    if (!ok) {
        std::cerr << "JSON parse error: " << errs << "\n";
        return 1;
    }

    // 3) CulturalEvent로 변환
    std::vector<CulturalEvent> events;
    const Json::Value arr = root.isMember("DATA") ? root["DATA"] : root;
    for (const auto& item : arr) {
        if (auto evOpt = CulturalEvent::tryFromJson(item)) {
            events.push_back(*evOpt);
        }
    }
    if (events.empty()) {
        std::cerr << "변환된 이벤트가 없습니다.\n";
        return 1;
    }

    // 4) 사용자 위치 (IP 기반; 실패 시 서울시청 좌표로 대체)
    GeoLocation me;
    if (auto meOpt = GeoIP::getUserLocation()) {
        me = *meOpt;
        std::cout << "내 위치: " << me.toString(6) << "\n";
    }
    else {
        me = GeoLocation(37.566535, 126.977969); // fallback
        std::cout << "내 위치(기본값 사용): " << me.toString(6) << "\n";
    }

    // 5) 각 이벤트까지 거리 계산 및 정렬
    struct Row { CulturalEvent ev; double km; };
    std::vector<Row> rows;
    rows.reserve(events.size());
    for (const auto& ev : events) {
        double km = me.distanceToKm(ev.location);
        rows.push_back({ ev, km });
    }
    std::sort(rows.begin(), rows.end(), [](const Row& a, const Row& b) { return a.km < b.km; });

    // 6) 출력
    std::cout << "\n=== 가까운 순 추천 ===\n";
    for (size_t i = 0; i < rows.size(); ++i) {
        const auto& r = rows[i];
        std::cout << std::fixed << std::setprecision(2)
            << i + 1 << ". [" << r.ev.codeName << "] " << r.ev.title
            << " @ " << r.ev.place
            << " | " << r.ev.date
            << " | " << r.km << " km"
            << " | (" << std::setprecision(6)
            << r.ev.location.latitude() << ", " << r.ev.location.longitude() << ")\n";
    }

    return 0;
}
