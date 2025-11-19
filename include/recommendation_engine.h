#pragma once
#include <vector>
#include "event_repository.h"
#include "geo.h"

struct RecommendedEvent {
    CulturalEvent event;
    double distanceKm;
};

class RecommendationEngine {
private:
    EventRepository& repository;

public:
    RecommendationEngine(EventRepository& repo);

    std::vector<RecommendedEvent> recommendByLocation(const GeoLocation& userLoc, double radius);
    std::vector<CulturalEvent>    recommendByDistrict(const std::string& districtName);
};
