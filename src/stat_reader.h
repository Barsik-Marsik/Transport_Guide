#pragma once

#include "transport_catalogue.h"

#include <iostream>
#include <set>
#include <string>
#include <vector>


struct BusInfo {
    std::string name = "";
    int count_stops = 0;
    int unique_stops = 0;
    double route_length = 0;
};

struct StopInfo {
	std::string_view name = "";
	bool existence = false;
	std::map<std::string_view, Bus*> buses = {};
};

std::ostream& operator<<(std::ostream& output, const BusInfo& bus);

std::ostream& operator<<(std::ostream& output, const StopInfo& stop);

int CalculateUniqueStops(const std::vector<Stop*>& stops_by_bus);

double CalculateRouteLength(const std::vector<Stop*>& stops_by_bus);

BusInfo GetBusInfoToStream(const TransportCatalogue& transport_cataloque, std::string_view bus_name);

StopInfo GetStopInfoToStream(const TransportCatalogue& transport_catalogue, std::string_view stop_name);
       
void PrintBusInfo(std::ostream& output, const BusInfo& bus_info);
