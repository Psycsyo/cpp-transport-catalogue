#pragma once
#include "json.h"
#include "transport_catalogue.h"
#include "map_renderer.h"
#include "request_handler.h"
#include "json_builder.h"

#include <sstream>

class JsonReader {
public:
    JsonReader(std::istream& input) : input_(json::Load(input)) {}

    void ProcessRequests(const json::Node& stat_requests, RequestHandler& request) const;

    const json::Node& GetDataRequests() const;
    const json::Node& GetRenderSettings() const;
    const json::Node& GetRoutingSettings() const;

    void FillCatalogue(transport::Catalogue& catalogue);
    renderer::MapRenderer FillRenderSettings(const json::Dict& request_map) const;
    transport::Router FillRoutingSettings(const json::Node& settings) const;
private:
    json::Document input_;
    json::Node dummy_;

    const json::Node& GetBaseRequests() const;

    json_reader::Stop FillStop(const json::Dict& request_map) const;
    json_reader::Bus FillBus(const json::Dict& request_map) const;

    const json::Node PrintBus(const json::Dict& request_map, RequestHandler& request) const;
    const json::Node PrintStop(const json::Dict& request_map, RequestHandler& request) const;
    const json::Node PrintMap(const json::Dict& request_map, RequestHandler& request) const;
    const json::Node PrintRoute(const json::Dict& request_map, RequestHandler& request) const;

    svg::Color GetColor(const json::Node& color_element) const;
};
