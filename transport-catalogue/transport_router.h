#pragma once

#include "router.h"
#include "transport_catalogue.h"

#include <memory>
#include <map>

namespace transport {

    struct RouterSettings {
        int bus_wait_time = 0;
        double bus_velocity = 0.0;
    };

    class Router {
    public:
        Router() = default;

        Router(const RouterSettings& settings)
            : settings_(settings) {}

        Router(const Router& settings, const Catalogue& catalogue) {
            settings_ = settings.settings_;
            BuildGraph(catalogue);
        }

        const std::optional<graph::Router<double>::RouteInfo> FindRoute(const std::string_view stop_from, const std::string_view stop_to) const;

    private:
        RouterSettings settings_;
        graph::DirectedWeightedGraph<double> graph_;
        std::map<std::string, graph::VertexId> stop_ids_;
        std::unique_ptr<graph::Router<double>> router_;

        void BuildGraph(const Catalogue& catalogue);
        const graph::DirectedWeightedGraph<double>& GetGraph() const;
    };

} 
