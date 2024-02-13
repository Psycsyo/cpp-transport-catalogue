#pragma once

#include "json.h"
#include "transport_catalogue.h"
#include "map_renderer.h"

class RequestHandler {
public:
    RequestHandler(const transport::Catalogue& catalogue, const renderer::MapRenderer& renderer) 
        : catalogue_(catalogue), renderer_(renderer) {}

    svg::Document RenderMap() const;

    bool HasBus(const std::string_view bus_number) const;
    bool HasStop(const std::string_view stop_name) const;

    std::optional<transport::data::Bus> GetBusData(const std::string bus_number) const;
    const std::set<std::string> GetBusesByStop(std::string_view stop_name) const;
private:
    const transport::Catalogue& catalogue_;
    const renderer::MapRenderer& renderer_;

};
