
#pragma once
#include <vector>
#include "event_repository.h"
#include "geo.h"

class RecommendationEngine {
private:
    EventRepository& repository;

public:
    RecommendationEngine(EventRepository& repo);

    std::vector<CulturalEvent> recommendByLocation(const GeoLocation& userLoc, double radius);
    std::vector<CulturalEvent> recommendByDistrict(const std::string& districtName);
};
