#include "stat_reader.h"

#include <execution>
#include <unordered_set>


std::ostream& operator<<(std::ostream& output, const BusInfo& bus) {
    output << "Bus "
            << bus.name << ": ";
    if (!bus.count_stops) {
    	return output << "not found";
    }
    output << bus.count_stops << " stops on route, "
            << bus.unique_stops << " unique stops, "
            << bus.route_length << " route length";
    return output;
}


struct HasherStop {
    size_t operator()(const Stop& stop) const {
        return hasher(stop.name);
    }

    std::hash<std::string> hasher;
};

int CalculateUniqueStops(const std::vector<Stop*>& stops_by_bus) {
    std::unordered_set<Stop, HasherStop> unique_stop;
    for (const auto& stop : stops_by_bus) {
        unique_stop.insert(*stop);
    }
    return static_cast<int>(unique_stop.size());
}

double CalculateRouteLength(const std::vector<Stop*>& stops_by_bus) {
    std::vector<Coordinates> coordinates(stops_by_bus.size());
    std::transform(std::execution::seq, stops_by_bus.begin(), stops_by_bus.end(),
                            coordinates.begin(), 
                            [](Stop* stop) {return stop->coordinates;});
    double distances = 0.0;
    for (int i = 0; i < static_cast<int>(coordinates.size()) - 1; ++i) {
        distances += ComputeDistance(coordinates[i], coordinates[i + 1]);
    }
    return distances;
}

BusInfo GetBusInfoToStream(const TransportCatalogue& transport_catalogue, std::string_view bus_name) {
    BusInfo bus_info;
    // Writing name
    bus_info.name = bus_name;
    const std::vector<Stop*>& stops_by_bus = transport_catalogue.GetBusInfo(bus_name);
    if (stops_by_bus.empty()) {
    	return bus_info;
    }
    // Writing count_stops
    bus_info.count_stops = stops_by_bus.size();
    // Writing unique stops
    bus_info.unique_stops = CalculateUniqueStops(stops_by_bus);
    // Writing route_length stops
    bus_info.route_length = CalculateRouteLength(stops_by_bus);
    return bus_info;
}

//void PrintBusInfo(std::ostream& output, const BusInfo& bus_info) {
//}