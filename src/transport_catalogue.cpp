#include "transport_catalogue.h"
#include "stat_reader.h"

#include <algorithm>


bool operator<(const Bus lhs, const Bus rhs) {
	return lhs.name < rhs.name;
}

std::ostream& operator<<(std::ostream& output, const Stop& stop) {
	output << stop.name << ": "
			<< stop.coordinates.lat << ", "
			<< stop.coordinates.lng;
	return output;
}

std::ostream& operator<<(std::ostream& output, const Bus& bus) {
	output << bus.name << ": ";
	for (const auto stop : bus.stops) {
		output << stop->name << " ";
	}
	return output;
}

bool operator==(const Stop& lh, const Stop& rh) {
    return lh.name == rh.name
        && lh.coordinates == rh.coordinates;
}

bool operator==(Stop& lh, Stop& rh) {
    return lh.name == rh.name
        && lh.coordinates == rh.coordinates;
}

void TransportCatalogue::AddStop(Stop new_stop) {
//    auto stop_ptr = stops_.emplace_back(std::move(new_stop));
    stops_.push_back(std::move(new_stop));
    stopname_to_stop_.insert({stops_.back().name, &stops_.back()});
}

void TransportCatalogue::AddBus(std::pair<std::string, std::vector<std::string>> new_bus) {
//	std::cout << "void TransportCatalogue::AddBus\n";
//	Print("stops");
//	std::cout << "stopname_to_stop_.size()=" << stopname_to_stop_.size() << std::endl;
//	Print("buses");
//	std::cout << "busname_to_bus_.size()=" << busname_to_bus_.size() << std::endl;

	Bus bus;
    bus.name = std::move(new_bus.first);
	std::string type_route = new_bus.second.back();
	new_bus.second.pop_back();
	for (std::string& stop : new_bus.second) {
		std::string_view stop_view = stop;
//		if (!buses_by_stop_.count(stop_view)) {
//			buses_by_stop_.insert({stopname_to_stop_.at(stop_view)->name, {{}}});
//		}
//		Print("buses by stop");
		bus.stops.push_back(stopname_to_stop_.at(stop_view));
	}
	// add circular route
    if (type_route == "-") {
    	for (auto i = bus.stops.size() - 1; i > 0; --i) {
    		bus.stops.push_back(bus.stops[i - 1]);
    	}
    }
    buses_.push_back(std::move(bus));
    busname_to_bus_.insert({buses_.back().name, &buses_.back()});
    // Add bus to stops
	for (std::string& stop : new_bus.second) {
		std::string_view stop_view = stop;
		buses_by_stop_[stopname_to_stop_.at(stop_view)->name][buses_.back().name] = &buses_.back();
	}
//    Print("buses by stop");
//    Print("buses");
}

Stop* TransportCatalogue::GetStop(std::string_view stop_name) const {
    if (stopname_to_stop_.count(stop_name)) {
    	return stopname_to_stop_.at(stop_name);
    }
	return nullptr;
}

const Bus* TransportCatalogue::GetBus(std::string_view bus_name) const {
	if (busname_to_bus_.count(bus_name)) {
		return busname_to_bus_.at(bus_name);
	}
    return nullptr;
}

std::vector<Stop*> TransportCatalogue::GetBusInfo(std::string_view bus_name) const {
    if (busname_to_bus_.count(bus_name)) {
    	return busname_to_bus_.at(bus_name)->stops;
    }
	return {};
}

std::map<std::string_view, Bus*> TransportCatalogue::GetStopInfo(std::string_view stop_name) const {
    if (buses_by_stop_.count(stop_name)) {
    	return buses_by_stop_.at(stop_name);
    }
	return {};
}

void TransportCatalogue::Print(const std::string& name) const {
	if (name == "stops") {
		for (const auto& stop : stops_) {
			std::cout << stop << " ";
		}
		std::cout << "\n";
		for (const auto& item : stopname_to_stop_) {
			std::cout << "[" << item.first << "]=" << item.second << " ";
		}
		std::cout << "\n";
	}
	else if (name == "buses") {
		for (const auto& stop : buses_) {
			std::cout << stop << " ";
		}
		std::cout << "\n";
		for (const auto& item : busname_to_bus_) {
			std::cout << "[" << item.first << "]=" << item.second << " ";
		}
		std::cout << "\n";
	}
	else if (name == "buses by stop") {
		std::cout << "buses_by_stop_: ";
		for (const auto& [stop, buses] : buses_by_stop_) {
			std::cout << stop << ":";
			for (const auto& bus : buses) {
				std::cout << " " << bus.first;
			}
			std::cout << std::endl;
		}
	}
	std::cout << std::endl;
}
