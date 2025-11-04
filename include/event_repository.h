#pragma once
#include <vector>
#include <string>
#include <algorithm>
#include <stdexcept>
#include "CulturalEvent.h"

// 인메모리 저장소(Repository) — 헤더 온리
class EventRepository {
public:
    enum class DuplicatePolicy { Throw, Upsert, Ignore };

    explicit EventRepository(DuplicatePolicy policy = DuplicatePolicy::Throw)
        : policy_(policy) {
    }

    // 이벤트 추가: 중복 ID 처리 정책 적용
    void addEvent(const CulturalEvent& event) {
        auto it = std::find_if(events_.begin(), events_.end(),
            [&](const CulturalEvent& e) { return e.id == event.id; });

        if (it == events_.end()) {            // 신규
            events_.push_back(event);
            return;
        }
        // 중복 처리
        switch (policy_) {
        case DuplicatePolicy::Throw:
            throw std::runtime_error("Duplicate event id: " + event.id);
        case DuplicatePolicy::Upsert:
            *it = event;                  // 덮어쓰기
            return;
        case DuplicatePolicy::Ignore:
            return;                       // 무시
        }
    }

    // 단건 조회: 없으면 nullptr
    const CulturalEvent* findById(const std::string& id) const {
        auto it = std::find_if(events_.begin(), events_.end(),
            [&](const CulturalEvent& e) { return e.id == id; });
        return (it == events_.end()) ? nullptr : &(*it);
    }

    // 전체 조회(값 복사)
    std::vector<CulturalEvent> findAll() const {
        return events_;
    }

    // 편의 기능
    bool removeById(const std::string& id) {
        auto it = std::find_if(events_.begin(), events_.end(),
            [&](const CulturalEvent& e) { return e.id == id; });
        if (it == events_.end()) return false;
        events_.erase(it);
        return true;
    }

    std::size_t size() const noexcept { return events_.size(); }
    void clear() noexcept { events_.clear(); }

private:
    std::vector<CulturalEvent> events_;
    DuplicatePolicy policy_;
};
