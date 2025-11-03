
#include <iostream>
#include <cstdlib>
#include "geo.h"

// 사용법:
//   ./program lat1 lon1 lat2 lon2
// 예시(서울 시청 근처에서 북쪽으로 약간 이동한 좌표):
//   ./program 37.5665 126.9780 37.5700 126.9768
int main(int argc, char* argv[]) {
    if (argc == 5) {
        double lat1 = std::atof(argv[1]);
        double lon1 = std::atof(argv[2]);
        double lat2 = std::atof(argv[3]);
        double lon2 = std::atof(argv[4]);
        double d = haversine_km(lat1, lon1, lat2, lon2);
        std::cout << "Distance (km): " << d << std::endl;
        return 0;
    }

    std::cout << "Seoul Culture Event Recommender (console starter)\n";
    std::cout << "Usage: " << argv[0] << " <lat1> <lon1> <lat2> <lon2>\n";
    std::cout << "Example: " << argv[0] << " 37.5665 126.9780 37.5700 126.9768\n";
    return 0;
}
