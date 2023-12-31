#include "input_reader.h"
#include "stat_reader.h"

#include <cassert>
#include <iomanip>
#include <stack>
#include <sstream>


std::ostream& operator<<(std::ostream& output, const Request& request) {
    output << "type=" << request.type
            << ", name=" << request.name
            << ", text='" << request.text
            << "'";
    return output;
}


std::string ReadLine(std::istream& input) {
    std::string s;
    std::getline(input, s, '\n');
    return s;
}

RequestQueue::RequestQueue(TransportCatalogue& transport_catalogue)
    : transport_catalogue_(transport_catalogue)
{
}

void RequestQueue::ReadRawRequests(std::istream& input, int num) {
    for (int i = 0; i < num; ++i) {
        AddRequest(ReadLine(input));
    }
}

void RequestQueue::AddRequest(std::string raw_request) {
    Request request = ParseRawRequest(std::move(raw_request));
    if (request.type == RequestType::Bus_r or 
        request.type == RequestType::Stop_r) {
        output_requests_.push_back(std::move(request));
    }
    else {
        input_requests_.push_back(std::move(request));
    }
}

void RequestQueue::ProcessInputRequests() {
    for (Request& request : input_requests_) {
        if (request.type == RequestType::Stop_w) {
            transport_catalogue_.AddStop(ParseStopRequest(request));
            // request must be deleted
        }
    }
    for (auto& [name, raw_distance] : raw_distances_) {
        transport_catalogue_.AddDistances(std::move(name), ParseDistances(raw_distance));
    }
    for (Request& request : input_requests_) {
        if (request.type == RequestType::Bus_w) {
            transport_catalogue_.AddBus(ParseBusRequest(request));
            // request must be deleted
        }
    }
}

void RequestQueue::ProcessOutputRequests(std::ostream& output) {
    for (Request& request : output_requests_) {
    if (request.type == RequestType::Bus_r) {
            output << GetBusInfoToStream(transport_catalogue_, ParseOutputRequest(request)) << "\n";
        }
    if (request.type == RequestType::Stop_r) {
            output << GetStopInfoToStream(transport_catalogue_, ParseOutputRequest(request)) << "\n";
        }
    }
}

void RequestQueue::PrintAllRequests(std::ostream& output) {
    output << "Input requests: \n";
    if (!input_requests_.empty()) {
        for (const auto& r : input_requests_) {
            output << r << "\n";
        }
    }
    else {
        output << "No input requests\n";
    }
    output << "Output requests: \n";
    if (!output_requests_.empty()) {
        for (const auto& r : output_requests_) {
            output << r << "\n";
        }
    }
    else {
        output << "No output requests\n";
    }
}

Request RequestQueue::ParseRawRequest(std::string text) {
//    std::cout << text << std::endl;
    Request request;
    std::istringstream input(text);
    std::string str;
    input >> str;
    if (str == "Stop") {
        std::getline(input, str, ':');
        // writing type request
        if (!input.eof()) {
            RemoveSpace(str);
            request.name = std::move(str);
            request.type = RequestType::Stop_w;
            std::getline(input, str);
            RemoveSpace(str);
            request.text = std::move(str);
        }
        else {
            RemoveSpace(str);
            request.name = std::move(str);
            request.type = RequestType::Stop_r;            
        }
    }
    else if (str == "Bus") {
        std::getline(input, str, ':');
        // writing type request
        if (!input.eof()) {
            RemoveSpace(str);
            request.name = std::move(str);
            request.type = RequestType::Bus_w;
            std::getline(input, str);
            RemoveSpace(str);
            request.text = std::move(str);
        }
        // reading type request
        else{
            RemoveSpace(str);
            request.name = std::move(str);
            request.type = RequestType::Bus_r;
        }
    }
    else {
        std::cout << "Error: Unknown command" << std::endl;
    }
    return request;
}

void RequestQueue::RemoveSpace(std::string& str) {
    auto begin = str.find_first_not_of(" ");
    auto end = str.find_last_not_of(" ");
    str = str.substr(begin, end - begin + 1);    
}

Stop RequestQueue::ParseStopRequest(Request& stop_request) {
    Stop stop;
    stop.name = stop_request.name;
    std::istringstream input(stop_request.text);
    input >> stop.coordinates.lat;
    input.ignore(1);        // ignore ','
    input >> stop.coordinates.lng;
    input.ignore(1);
    std::string raw_distance;
    std::getline(input, raw_distance);
    if (!raw_distance.empty()) {
        raw_distances_.push_back({stop.name, std::move(raw_distance)});
    }
    return stop;
}

std::vector<std::pair<std::string, int>> RequestQueue::ParseDistances(std::string raw_distance) {
    std::string text;
    std::vector<std::pair<std::string, int>> distances;
    std::istringstream input(raw_distance);
    while (std::getline(input, text, ',')) {
        auto m = text.find_first_of('m');
        int distance = stoi(text.substr(0, m));
        text = std::move(text.substr(m + 5));
        RemoveSpace(text);
        distances.push_back({std::move(text), distance});
    }
    return distances;
}

std::pair<std::string, std::vector<std::string>> RequestQueue::ParseBusRequest(Request& bus_request) {
    std::string text = std::move(bus_request.text);
    std::string name_bus = std::move(bus_request.name);
    std::vector<std::string> stops;
    // search separator symbol
    auto pos = std::min(text.find('>'), text.find('-'));
    std::string route_type = text.substr(pos, 1);
    // parsing stop type
    pos = 0;
    while (true) {
        auto begin = text.find_first_not_of(" ", pos);
        pos = text.find(route_type, begin);
        auto end = text.find_last_not_of(" ", pos - 1);
        stops.push_back(text.substr(begin, end - begin + 1));
        if (pos != std::string::npos) {
            ++pos;
            continue;
        }
        break;
    }
    stops.push_back(std::move(route_type));
    return {name_bus, stops};
}

std::string_view RequestQueue::ParseOutputRequest(const Request& output_request) {
    return output_request.name;
}
