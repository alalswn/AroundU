#pragma once
#include <cmath>
#include <string>
#include <stdexcept>
#include <sstream>
#include <iomanip>

class GeoLocation {
public:
    constexpr GeoLocation() noexcept : lat_(0.0), lon_(0.0) {}
    GeoLocation(double latitudeDeg, double longitudeDeg) : lat_(latitudeDeg), lon_(longitudeDeg) {
        validate();
    }
    constexpr double latitude()  const noexcept { return lat_; }
    constexpr double longitude() const noexcept { return lon_; }
    std::string toString(int precision = 6) const {
        std::ostringstream oss;
        oss << "GeoLocation(" << std::fixed << std::setprecision(precision)
            << lat_ << ", " << lon_ << ")";
        return oss.str();
    }
    double distanceToKm(const GeoLocation& other) const noexcept {
        constexpr double R = 6371.0;
        const double lat1 = deg2rad(lat_), lat2 = deg2rad(other.lat_);
        const double dlat = lat2 - lat1;
        const double dlon = deg2rad(other.lon_ - lon_);
        const double a = std::sin(dlat / 2) * std::sin(dlat / 2)
            + std::cos(lat1) * std::cos(lat2) * std::sin(dlon / 2) * std::sin(dlon / 2);
        const double c = 2.0 * std::atan2(std::sqrt(a), std::sqrt(1.0 - a));
        return R * c;
    }
    double distanceToMeters(const GeoLocation& other) const noexcept {
        return distanceToKm(other) * 1000.0;
    }

private:
    double lat_, lon_;
    static constexpr double pi() noexcept { return 3.14159265358979323846; }
    static constexpr double deg2rad(double d) noexcept { return d * (pi() / 180.0); }
    void validate() const {
        if (lat_ < -90.0 || lat_ > 90.0)   throw std::invalid_argument("latitude in [-90,90]");
        if (lon_ < -180.0 || lon_ > 180.0) throw std::invalid_argument("longitude in [-180,180]");
    }
};
