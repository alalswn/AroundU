#pragma once
#include <cmath>

class GeoLocation {
public:
    double latitude;
    double longitude;

    GeoLocation() : latitude(0.0), longitude(0.0) {}
    GeoLocation(double lat, double lon) : latitude(lat), longitude(lon) {}

    // 두 위치 사이 거리 (km)
    double distanceTo(const GeoLocation& other) const;
};
