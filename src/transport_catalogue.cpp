#include "transport_catalogue.h"
#include "stat_reader.h"

#include <algorithm>


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
//	TransportCatalogue::Print("stops");
//	std::cout << "stopname_to_stop_.size()=" << stopname_to_stop_.size() << std::endl;
//	TransportCatalogue::Print("buses");
//	std::cout << "busname_to_bus_.size()=" << busname_to_bus_.size() << std::endl;

	Bus bus;
    bus.name = std::move(new_bus.first);
	std::string type_route = new_bus.second.back();
	new_bus.second.pop_back();
	for (std::string& stop : new_bus.second) {
		bus.stops.push_back(stopname_to_stop_.at(std::move(stop)));
	}
	// add circular route
    if (type_route == "-") {
    	for (auto i = bus.stops.size() - 1; i > 0; --i) {
    		bus.stops.push_back(bus.stops[i - 1]);
    	}
    }
	buses_.push_back(std::move(bus));
    busname_to_bus_.insert({buses_.back().name, &buses_.back()});
//    Print("buses");
}

Stop* TransportCatalogue::GetStop(std::string_view name_stop) const {
    return stopname_to_stop_.at(name_stop);
}

const Bus* TransportCatalogue::GetBus(std::string_view name_bus) const {
    return busname_to_bus_.at(name_bus);
}

std::vector<Stop*> TransportCatalogue::GetBusInfo(std::string_view name_bus) const {
    if (busname_to_bus_.count(name_bus)) {
    	return busname_to_bus_.at(name_bus)->stops;
    }
    // bus not found
//    std::vector<Stop*> bus_not_found = {};
	return {};
}

/*
std::pair<std::string_view, std::vector<std::string_view>> TransportCatalogue::GetBusInfo(
		std::string_view name_bus) const {
	std::vector<std::string_view> stops;
	for (const auto& stop : busname_to_bus_.at(name_bus)) {
		stops.push_back(&stop);
	}
	std::cout << "GetBusInfo: \n";
	for (const auto stop : stops) {
		std::cout << stop << " ";
	}
	return {busname_to_bus_.at(name_bus)->name, stops};
}
*/
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
	std::cout << std::endl;
}
