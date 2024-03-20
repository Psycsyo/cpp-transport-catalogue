#include "json_reader.h"

void JsonReader::ProcessRequests(const json::Node& stat_requests, RequestHandler& request_handler) const {
    json::Array result;
    for (auto& request : stat_requests.AsArray()) {
        const auto& request_map = request.AsDict();
        if (!request_map.count("type")) {
            continue;
        }

        const auto& type = request_map.at("type").AsString();
        if (type == "Stop") {
            result.push_back(PrintStop(request_map, request_handler).AsDict());
        }
        if (type == "Bus") {
            result.push_back(PrintBus(request_map, request_handler).AsDict());
        }
        if (type == "Map") {
            result.push_back(PrintMap(request_map, request_handler).AsDict());
        }
        if (type == "Route") {
            result.push_back(PrintRoute(request_map, request_handler).AsDict());
        }
    }

    json::Print(json::Document{ result }, std::cout);
}

const json::Node& JsonReader::GetDataRequests() const {
    if (const auto* ptr = std::get_if<json::Dict>(&input_.GetRoot().GetValue())) {
        if (auto it = ptr->find("stat_requests"); it != ptr->end()) {
            return it->second;
        }
    }
    return dummy_;    
}

const json::Node& JsonReader::GetRenderSettings() const {
    if (const auto* ptr = std::get_if<json::Dict>(&input_.GetRoot().GetValue())) {
        if (auto it = ptr->find("render_settings"); it != ptr->end()) {
            return it->second;
        }
    }
    return dummy_;
}

const json::Node& JsonReader::GetRoutingSettings() const {
    if (const auto* ptr = std::get_if<json::Dict>(&input_.GetRoot().GetValue())) {
        if (auto it = ptr->find("routing_settings"); it != ptr->end()) {
            return it->second;
        }
    }
    return dummy_;
}

const json::Node& JsonReader::GetBaseRequests() const {
    if (const auto* ptr = std::get_if<json::Dict>(&input_.GetRoot().GetValue())) {
        if (auto it = ptr->find("base_requests"); it != ptr->end()) {
            return it->second;
        }
    }
    return dummy_;
}

void JsonReader::FillCatalogue(transport::Catalogue& catalogue) {
    const json::Array& arr = GetBaseRequests().AsArray();
    std::vector<json_reader::Stop> query_stop;
    std::vector<json_reader::Bus> query_bus;

    for (auto& request_stops : arr) {
        const auto& request_stops_map = request_stops.AsDict();
        const auto& type = request_stops_map.at("type").AsString();
        if (type == "Stop") {
            query_stop.push_back(FillStop(request_stops_map));
        }
        if (type == "Bus") {
            query_bus.push_back(FillBus(request_stops_map));
        }
    }

    for (auto& stop : query_stop) {
        catalogue.AddStop(stop.stop_name, stop.coordinates);
    }

    for (auto& stop : query_stop) {
        for (const auto& distance : stop.stop_distances) {
            catalogue.SetDistance(catalogue.FindStop(stop.stop_name), catalogue.FindStop(distance.first), distance.second);
        }
    }

    for (auto& bus : query_bus) {
        catalogue.AddBus(bus.bus_number, bus.stops, bus.is_circle);
    }
}

renderer::MapRenderer JsonReader::FillRenderSettings(const json::Dict& request_map) const {
    renderer::RenderSettings render_settings;
    render_settings.width = request_map.at("width").AsDouble();
    render_settings.height = request_map.at("height").AsDouble();
    render_settings.padding = request_map.at("padding").AsDouble();
    render_settings.stop_radius = request_map.at("stop_radius").AsDouble();
    render_settings.line_width = request_map.at("line_width").AsDouble();
    render_settings.bus_label_font_size = request_map.at("bus_label_font_size").AsInt();

    const json::Array& bus_label_offset = request_map.at("bus_label_offset").AsArray();
    render_settings.bus_label_offset = { bus_label_offset[0].AsDouble(), bus_label_offset[1].AsDouble() };
    render_settings.stop_label_font_size = request_map.at("stop_label_font_size").AsInt();

    const json::Array& stop_label_offset = request_map.at("stop_label_offset").AsArray();
    render_settings.stop_label_offset = { stop_label_offset[0].AsDouble(), stop_label_offset[1].AsDouble() };
    render_settings.underlayer_color = GetColor(request_map.at("underlayer_color"));
    render_settings.underlayer_width = request_map.at("underlayer_width").AsDouble();

    const json::Array& color_palette = request_map.at("color_palette").AsArray();
    for (const auto& color_element : color_palette) {
        render_settings.color_palette.emplace_back(GetColor(color_element));
    }

    return render_settings;
}

transport::Router JsonReader::FillRoutingSettings(const json::Node& settings) const {
    return transport::Router{ settings.AsDict().at("bus_wait_time").AsInt(), settings.AsDict().at("bus_velocity").AsDouble() };
}

json_reader::Stop JsonReader::FillStop(const json::Dict& request_map) const {
    std::string stop_name = request_map.at("name").AsString();
    geo::Coordinates coordinates = { request_map.at("latitude").AsDouble(), request_map.at("longitude").AsDouble() };
    std::map<std::string, int> stop_distances;

    auto& distances = request_map.at("road_distances").AsDict();
    for (auto& [stop_name, dist] : distances) {
        stop_distances.emplace(stop_name, dist.AsInt());
    }

    return { stop_name, coordinates, stop_distances };
}

json_reader::Bus JsonReader::FillBus(const json::Dict& request_map) const {
    std::string bus_number = request_map.at("name").AsString();
    std::vector<std::string> stops;
    for (auto& stop : request_map.at("stops").AsArray()) {
        stops.push_back(stop.AsString());
    }

    bool circular_route = request_map.at("is_roundtrip").AsBool();
    return { bus_number, stops, circular_route };
}

const json::Node JsonReader::PrintBus(const json::Dict& request_map, RequestHandler& request) const {
    json::Dict result;
    const std::string& bus_number = request_map.at("name").AsString();
    result["request_id"] = request_map.at("id").AsInt();
    if (!request.HasBus(bus_number)) {
        result["error_message"] = json::Node{ static_cast<std::string>("not found") };
    }
    else {
        result["curvature"] = request.GetBusData(bus_number).value().curvature;
        result["route_length"] = request.GetBusData(bus_number).value().route_length;
        result["stop_count"] = static_cast<int>(request.GetBusData(bus_number).value().stop_count);
        result["unique_stop_count"] = static_cast<int>(request.GetBusData(bus_number).value().unique_stop_count);
    }

    return json::Node{ result };
}

const json::Node JsonReader::PrintStop(const json::Dict& request_map, RequestHandler& request) const { 
    json::Dict result;
    result["request_id"] = request_map.at("id").AsInt();

    const std::string& stop_name = request_map.at("name").AsString();
    if (!request.HasStop(stop_name)) {
        result["error_message"] = json::Node{ static_cast<std::string>("not found") };
    }
    else {
        json::Array buses;
        for (auto& bus : request.GetBusesByStop(stop_name)) {
            buses.push_back(bus);
        }
        result["buses"] = buses;
    }

    return json::Node{ result };
}

const json::Node JsonReader::PrintMap(const json::Dict& request_map, RequestHandler& request) const {
    json::Dict result;
    result["request_id"] = request_map.at("id").AsInt();
    std::ostringstream strm;
    svg::Document map = request.RenderMap();
    map.Render(strm);
    result["map"] = strm.str();

    return json::Node{ result };
}

int CountSpan() {
    return 0;
}

const json::Node JsonReader::PrintRoute(const json::Dict& request_map, RequestHandler& request) const {
    json::Node result;
    const int id = request_map.at("id").AsInt();
    const std::string stop_from = request_map.at("from").AsString();
    const std::string stop_to = request_map.at("to").AsString();
    const auto& routing = request.GetOptimalRoute(stop_from, stop_to);

    if (!routing) {
        result = json::Builder{}
        .StartDict()
            .Key("request_id").Value(id)
            .Key("error_message").Value("not found")
        .EndDict()
        .Build();
    }
    else {
        json::Array items;
        double total_time = 0.0;
        items.reserve(routing.value().edges.size());

        for (auto& edge_id : routing.value().edges) {
            const graph::Edge<double> edge = request.GetRouterGraph().GetEdge(edge_id);

            if (edge.data == 0) {
                items.emplace_back(json::Node(json::Builder{}
                        .StartDict()
                            .Key("stop_name").Value(edge.name)
                            .Key("time").Value(edge.weight)
                            .Key("type").Value("Wait")
                        .EndDict()
                    .Build()));

                total_time += edge.weight;
            }
            else {
                items.emplace_back(json::Node(json::Builder{}
                        .StartDict()
                            .Key("bus").Value(edge.name)
                            .Key("span_count").Value(static_cast<int>(edge.data))
                            .Key("time").Value(edge.weight)
                            .Key("type").Value("Bus")
                        .EndDict()
                    .Build()));

                total_time += edge.weight;
            }
        }

        result = json::Builder{}
                .StartDict()
                    .Key("request_id").Value(id)
                    .Key("total_time").Value(total_time)
                    .Key("items").Value(items)
                .EndDict()
            .Build();
    }

    return result;
}

svg::Color JsonReader::GetColor(const json::Node& color_element) const {
    if (color_element.IsString()) return color_element.AsString();
    else if (color_element.IsArray()) {
        const json::Array& color_type = color_element.AsArray();
        uint8_t a = static_cast<uint8_t>(color_type[0].AsInt());
        uint8_t b = static_cast<uint8_t>(color_type[1].AsInt());
        uint8_t c = static_cast<uint8_t>(color_type[2].AsInt());

        if (color_type.size() == 3) {
            return svg::Rgb(a, b, c);
        }
        else if (color_type.size() == 4) {
            return svg::Rgba(a, b, c, color_type[3].AsDouble());
        }
        else throw std::logic_error("wrong color_palette type");
    }
    else throw std::logic_error("wrong color_palette");
}
