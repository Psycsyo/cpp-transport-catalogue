#include "transport_router.h"

namespace transport {

    const graph::DirectedWeightedGraph<double>& Router::BuildGraph(const Catalogue& catalogue) {

        const auto& all_stops = catalogue.GetSortedAllStops();
        const auto& all_buses = catalogue.GetSortedAllBuses();

        graph::DirectedWeightedGraph<double> stops_graph(all_stops.size() * 2);
        std::map<std::string, graph::VertexId> stop_ids;
        graph::VertexId vertex_id = 0;

        FillStopsGraph(all_stops, stops_graph, stop_ids, vertex_id);
        stop_ids_ = std::move(stop_ids);

        FillBusesGraph(all_buses, stops_graph, stop_ids, catalogue);

        graph_ = std::move(stops_graph);
        router_ = std::make_unique<graph::Router<double>>(graph_);

        return graph_;
    }

    void Router::FillStopsGraph(const std::map<std::string, std::shared_ptr<type::Stop>>& all_stops,
                                graph::DirectedWeightedGraph<double>& stops_graph,
                                std::map<std::string, graph::VertexId>& stop_ids,
                                graph::VertexId& vertex_id) {
        for (const auto& [stop_name, stop_info] : all_stops) {
            stop_ids[stop_info->name] = vertex_id;
            stops_graph.AddEdge({
                stop_info->name,
                0,
                vertex_id,
                ++vertex_id,
                static_cast<double>(bus_wait_time_)
            });
            ++vertex_id;
        }
    }

    void Router::FillBusesGraph(const std::map<std::string, std::shared_ptr<type::Bus>>& all_buses,
                                graph::DirectedWeightedGraph<double>& stops_graph,
                                std::map<std::string, graph::VertexId>& stop_ids,
                                const Catalogue& catalogue) {
        for_each(all_buses.begin(), all_buses.end(), [&stops_graph, this, &catalogue](const auto& item) {
            const auto& bus_info = item.second;
            const auto& stops = bus_info->stops;
            size_t stops_count = stops.size();

            for (size_t i = 0; i < stops_count; ++i) {
                for (size_t j = i + 1; j < stops_count; ++j) {
                    double velocity_coefficient = 100.0 / 6.0;
                    stops_graph.AddEdge({
                        bus_info->number,
                        j - i,
                        stop_ids_.at(stop_from->name) + 1,
                        stop_ids_.at(stop_to->name),
                        static_cast<double>(dist_sum) / (bus_velocity_ * velocity_coefficient)
                    });
                }
            }
        });
    }

    const std::optional<graph::Router<double>::RouteInfo> Router::FindRoute(const std::string_view stop_from, const std::string_view stop_to) const {
        return router_->BuildRoute(stop_ids_.at(std::string(stop_from)), stop_ids_.at(std::string(stop_to));
    }

    const graph::DirectedWeightedGraph<double>& Router::GetGraph() const {
        return graph_;
    }

}
