#pragma once

#include "geo.h"

#include <deque>
#include <iostream>
#include <map>
#include <set>
#include <string>
#include <string_view>
#include <vector>
#include <unordered_map>


// Struct holding information about one stop
struct Stop {
    std::string name;
    Coordinates coordinates;
};

// Struct holding information about one Bus
struct Bus {
    std::string name;
    std::vector<Stop*> stops;
};

bool operator<(const Bus lhs, const Bus rhs);

std::ostream& operator<<(std::ostream& output, const Stop& stop);

std::ostream& operator<<(std::ostream& output, const Bus& bus);

bool operator==(const Stop& lh, const Stop& rh);

class TransportCatalogue {
public:
    void AddStop(Stop new_stop);

    void AddBus(std::pair<std::string, std::vector<std::string>> new_bus);

    Stop* GetStop(std::string_view name_stop) const;

    const Bus* GetBus(std::string_view name_bus) const;

    std::vector<Stop*> GetBusInfo(std::string_view bus_name) const;

    std::map<std::string_view, Bus*> GetStopInfo(std::string_view stop_name) const;

    void Print(const std::string& name) const;

private:
    std::deque<Stop> stops_;
    std::deque<Bus> buses_;
    std::unordered_map<std::string_view, Stop*> stopname_to_stop_;
    std::unordered_map<std::string_view, Bus*> busname_to_bus_;
    std::unordered_map<std::string_view, std::map<std::string_view, Bus*>> buses_by_stop_;
};
