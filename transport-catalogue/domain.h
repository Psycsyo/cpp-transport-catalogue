#pragma once

#include "geo.h"

#include <string>
#include <vector>
#include <set>
#include <map>
#include <unordered_map>

namespace transport {
	namespace type {
		struct Stop {
			std::string name;
			geo::Coordinates coordinates;
			std::set<std::string> buses_by_stop;
		};

		struct Bus {
			std::string number;
			std::vector<Stop*> stops;
			bool circle;
		};
	}

	namespace data {
		struct Stop {
			std::string name;
			std::set<std::string> buses_by_stop;
		};

		struct Bus {
			std::string name;
			unsigned int stop_count;
			unsigned int unique_stop_count;
			double route_length;
			double curvature;
		};
	}
}

namespace json_reader {
	struct Stop {
		std::string stop_name;
		geo::Coordinates coordinates;
		std::map<std::string, int> stop_distances;
	};

	struct Bus {
		std::string bus_number;
		std::vector<std::string> stops;
		bool is_circle;
	};
}
