
#include <cmath>
#include "geo.h"

static inline double deg2rad(double deg) {
    return deg * M_PI / 180.0;
}

double haversine_km(double lat1, double lon1, double lat2, double lon2) {
    const double R = 6371.0; // km
    const double dLat = deg2rad(lat2 - lat1);
    const double dLon = deg2rad(lon2 - lon1);
    const double a = std::sin(dLat/2) * std::sin(dLat/2) +
                     std::cos(deg2rad(lat1)) * std::cos(deg2rad(lat2)) *
                     std::sin(dLon/2) * std::sin(dLon/2);
    const double c = 2 * std::atan2(std::sqrt(a), std::sqrt(1.0 - a));
    return R * c;
}
