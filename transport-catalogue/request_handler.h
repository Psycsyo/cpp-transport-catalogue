#pragma once

#include "json.h"
#include "transport_catalogue.h"
#include "transport_router.h"
#include "map_renderer.h"

class RequestHandler {
public:
    RequestHandler(const transport::Catalogue& catalogue, const renderer::MapRenderer& renderer, const transport::Router& router)
        : catalogue_(catalogue), renderer_(renderer), router_(router) {}

    svg::Document RenderMap() const;

    bool HasBus(const std::string_view bus_number) const;
    bool HasStop(const std::string_view stop_name) const;

    std::optional<transport::data::Bus> GetBusData(const std::string bus_number) const;
    const std::set<std::string> GetBusesByStop(std::string_view stop_name) const;

    const std::optional<graph::Router<double>::RouteInfo> GetOptimalRoute(const std::string_view stop_from, const std::string_view stop_to) const;
    const graph::DirectedWeightedGraph<double>& GetRouterGraph() const;
private:
    const transport::Catalogue& catalogue_;
    const renderer::MapRenderer& renderer_;
    const transport::Router& router_;
};
