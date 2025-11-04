#pragma once
#include <vector>
#include <optional>
#include "cultural_event.h"

class EventRepository {
private:
    std::vector<CulturalEvent> events;

public:
    void addEvent(const CulturalEvent& e);
    std::optional<CulturalEvent> findById(const std::string& id) const;
    std::vector<CulturalEvent> findAll() const;
};

