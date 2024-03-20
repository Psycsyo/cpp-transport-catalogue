#pragma once
#include <deque>
#include <vector>
#include <string>
#include <set>
#include <optional>
#include <memory>
#include <unordered_map>
#include <unordered_set>

#include "geo.h"
#include "domain.h"

namespace transport {
	using namespace transport;

	class Catalogue {
	public:
		struct StopDistancesHasher {
			size_t operator()(const std::pair<const type::Stop*, const type::Stop*>& points) const {
				size_t hash_first = std::hash<const void*>{}(points.first);
				size_t hash_second = std::hash<const void*>{}(points.second);
				return hash_first + hash_second * 37;
			}
		};
		void AddStop(const std::string& name, const geo::Coordinates& coords);
		void AddBus(const std::string bus_number, const std::vector<std::string>& stops, const bool is_circle);
		
		type::Bus* FindBus(std::string_view bus_number) const;
		type::Stop* FindStop(std::string_view stop_name) const;
		std::set<std::string> FindStopsForBus(std::string_view bus_number) const;

		std::optional<data::Bus> GetBusData(std::string_view value) const;
		std::optional<data::Stop> GetStopData(std::string_view value) const;

		void SetDistance(type::Stop* from, type::Stop* to, int distance);
		int GetDistance(type::Stop* from, type::Stop* to) const;

		const std::map<std::string_view, const type::Bus*> GetSortedAllBuses() const;
		const std::map<std::string_view, const type::Stop*> GetSortedAllStops() const;
	private:
		std::deque<type::Bus> all_buses_;
		std::deque<type::Stop> all_stops_;

		std::unordered_map<std::string_view, type::Bus*> busname_to_bus_;
		std::unordered_map<std::string_view, type::Stop*> stopname_to_stop_;
		std::unordered_map<std::pair<type::Stop*, type::Stop*>, int, StopDistancesHasher> stop_distances_;

		size_t GetStopCount(std::string_view bus_number) const;
		size_t GetUiniqueStopsCount(std::string_view bus_number) const;
		double GetBusRouteDistance(std::string_view bus_number) const;
		double GetBusRouteCurvature(std::string_view bus_number) const;
	};
}
