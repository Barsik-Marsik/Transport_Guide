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
            << bus.route_geolength << " route length, "
            << bus.curvature << " curvature";
    return output;
}

std::ostream& operator<<(std::ostream& output, const StopInfo& stop) {
    output << "Stop "
            << stop.name << ": ";
    if (!stop.existence) {
        return output << "not found";
    }
    if (stop.buses.empty()) {
        return output << "no buses";
    }
    output << "buses";
    for (const auto& bus : stop.buses) {
        output << " " << bus.first;
    }
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

double CalculateRouteGeoLength(const std::vector<Stop*>& stops_by_bus) {
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

double CalculateRouteReallength(const TransportCatalogue& transport_catalogue, const std::vector<Stop*>& stops_by_bus) {
    double reallength = 0.0;
    for (int i = 0; i < static_cast<int>(stops_by_bus.size()) - 1; ++i) {
        reallength += transport_catalogue.GetDistance(stops_by_bus[i], stops_by_bus[i + 1]);
    }
    return reallength;
}

BusInfo GetBusInfoToStream(const TransportCatalogue& transport_catalogue, std::string_view bus_name) {
    BusInfo bus_info;
    // Writing name
    bus_info.name = bus_name;
    const std::vector<Stop*> stops_by_bus = transport_catalogue.GetBusInfo(bus_name);
    if (stops_by_bus.empty()) {
        return bus_info;
    }
    // Writing count_stops
    bus_info.count_stops = stops_by_bus.size();
    // Writing unique stops
    bus_info.unique_stops = CalculateUniqueStops(stops_by_bus);
    // Writing route_length stops
    bus_info.route_geolength = CalculateRouteReallength(transport_catalogue, stops_by_bus);
    bus_info.curvature = bus_info.route_geolength / CalculateRouteGeoLength(stops_by_bus);
    return bus_info;
}

StopInfo GetStopInfoToStream(const TransportCatalogue& transport_catalogue, std::string_view stop_name) {
    StopInfo stop_info;
    // Writing name
    stop_info.name = stop_name;
    if (transport_catalogue.GetStop(stop_name) == nullptr) {
        return stop_info;
    }
    stop_info.existence = true;
    stop_info.buses = transport_catalogue.GetStopInfo(stop_name);
    return stop_info;
}
