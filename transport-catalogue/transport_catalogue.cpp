#include "transport_catalogue.h"

namespace Transport {

void Catalogue::AddRoute(const std::string& route_number, const std::vector<std::string>& route_stops, bool circular_route) {
    all_buses_.push_back({ route_number, route_stops, circular_route });
    for (const auto& route_stop : route_stops) {
        for (auto& stop_ : all_stops_) {
            if (stop_.name == route_stop) stop_.buses.insert(route_number);
        }
    }
    busname_to_bus_[all_buses_.back().number] = &all_buses_.back();
}

void Catalogue::AddStop(const std::string& stop_name, geo::Coordinates& coordinates) {
    all_stops_.push_back({ stop_name, coordinates, {} });
    stopname_to_stop_[all_stops_.back().name] = &all_stops_.back();
}

const Bus* Catalogue::FindRoute(const std::string& route_number) const {
    return busname_to_bus_.count(route_number) ? busname_to_bus_.at(route_number) : nullptr;
}

const Stop* Catalogue::FindStop(const std::string& stop_name) const {
    return stopname_to_stop_.count(stop_name) ? stopname_to_stop_.at(stop_name) : nullptr;
}

const RouteInfo Catalogue::RouteInformation(const std::string& route_number) const {
    RouteInfo route_info{};
    const Bus* bus = FindRoute(route_number);

    if (!bus) throw std::invalid_argument("bus not found");
    if (bus->circular_route) route_info.stops_count = bus->stops->size();
    else route_info.stops_count = bus->stops->size() * 2 - 1;

    double route_length = 0.0;
    for (auto iter = bus->stops.value().begin(); iter + 1 != bus->stops.value().end(); ++iter) {
        if (bus->circular_route) route_length += ComputeDistance(stopname_to_stop_.find(*iter)->second->coordinates,
            stopname_to_stop_.find(*(iter + 1))->second->coordinates);
        else route_length += ComputeDistance(stopname_to_stop_.find(*iter)->second->coordinates,
            stopname_to_stop_.find(*(iter + 1))->second->coordinates) * 2;
    }
    route_info.unique_stops_count = UniqueStopsCount(route_number);
    route_info.route_length = route_length;

    return route_info;
}

size_t Catalogue::UniqueStopsCount(const std::string& route_number) const {
    std::unordered_set<std::string> unique_stops;
    for (const auto& stop : busname_to_bus_.at(route_number)->stops.value()) {
        unique_stops.insert(stop);
    }
    return unique_stops.size();
}

const std::set<std::string> Catalogue::GetBusesOnStop(const std::string& stop_name) const {
    return stopname_to_stop_.at(stop_name)->buses;
}
    
}
