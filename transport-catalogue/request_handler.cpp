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

const std::optional<graph::Router<double>::RouteInfo> RequestHandler::GetOptimalRoute(const std::string_view stop_from, const std::string_view stop_to) const {
    return router_.FindRoute(stop_from, stop_to);
}

const graph::DirectedWeightedGraph<double>& RequestHandler::GetRouterGraph() const {
    return router_.GetGraph();
}

bool RequestHandler::HasBus(const std::string_view bus_number) const {
    return catalogue_.FindBus(bus_number);
}

bool RequestHandler::HasStop(const std::string_view stop_name) const {
    return catalogue_.FindStop(stop_name);
}
