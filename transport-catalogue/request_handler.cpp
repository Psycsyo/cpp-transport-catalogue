#include "request_handler.h"

svg::Document RequestHandler::RenderMap() const {
    return renderer_.GetSVG(catalogue_.GetSortedAllBuses());
}

std::optional<transport::data::Bus> RequestHandler::GetBusData(const std::string bus_number) const {
    return catalogue_.GetBusData(bus_number);
}

const std::set<std::string> RequestHandler::GetBusesByStop(std::string_view stop_name) const {
    return catalogue_.FindStopsForBus(stop_name);
}

bool RequestHandler::HasBus(const std::string_view bus_number) const {
    return catalogue_.FindBus(bus_number);
}

bool RequestHandler::HasStop(const std::string_view stop_name) const {
    return catalogue_.FindStop(stop_name);
}
