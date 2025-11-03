
#include <cassert>
#include <cmath>
#include <iostream>
#include "geo.h"

int main() {
    // 간단한 정합성 테스트 (동일 좌표 -> 0km)
    double d0 = haversine_km(37.5665, 126.9780, 37.5665, 126.9780);
    assert(std::fabs(d0) < 1e-9);

    // 대략적인 범위 테스트 (서울 시청 근처 좌표에서 조금 이동)
    double d1 = haversine_km(37.5665, 126.9780, 37.5700, 126.9768);
    std::cout << "d1 ~ " << d1 << " km\n";
    assert(d1 > 0.1 && d1 < 1.0); // 대략적인 범위

    std::cout << "geo tests passed\n";
    return 0;
}
