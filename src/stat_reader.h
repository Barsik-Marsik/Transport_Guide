#pragma once

#include "transport_catalogue.h"

#include <iostream>
#include <string>
#include <vector>


struct BusInfo {
    std::string name;
    int count_stops = 0;
    int unique_stops = 0;
    double route_length = 0;
};

std::ostream& operator<<(std::ostream& output, const BusInfo& bus);

int CalculateUniqueStops(const std::vector<Stop*>& stops_by_bus);

double CalculateRouteLength(const std::vector<Stop*>& stops_by_bus);

BusInfo GetBusInfoToStream(const TransportCatalogue& transport_cataloque, std::string_view bus_name);
        //GetBusInfoToStream(transport_catalogue, output_request)
//void PrintBusInfo(std::ostream& output, const BusInfo& bus_info);
