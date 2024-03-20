#pragma once

#include <cmath>

const int constant = 6371000;
const double PI = std::atan(1.0) * 4;

namespace geo {

    struct Coordinates {
        double lat; // Широта
        double lng; // Долгота
        bool operator==(const Coordinates& other) const {
            return lat == other.lat && lng == other.lng;
        }
        bool operator!=(const Coordinates& other) const {
            return !(*this == other);
        }
    };

    double ComputeDistance(Coordinates from, Coordinates to);

}  // namespace geo
