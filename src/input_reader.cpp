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

RequestQueue::RequestQueue(TransportCatalogue& transport_cataloque)
    : transport_cataloque_(transport_cataloque)
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
            transport_cataloque_.AddStop(ParseStopRequest(request));
            // request must be deleted
        }
    }
    for (Request& request : input_requests_) {
        if (request.type == RequestType::Bus_w) {
            transport_cataloque_.AddBus(ParseBusRequest(request));
            // request must be deleted
        }
    }
}

void RequestQueue::ProcessOutputRequests(std::ostream& output) {
    for (Request& request : output_requests_) {
	if (request.type == RequestType::Bus_r) {
            output << GetBusInfoToStream(transport_cataloque_, ParseOutputRequest(request)) << "\n";
        }
	if (request.type == RequestType::Stop_r) {
            output << GetStopInfoToStream(transport_cataloque_, ParseOutputRequest(request)) << "\n";
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
//    std::cout << stop_request.text << std::endl;        // Delete!!!
    std::istringstream input(stop_request.text);
    input >> stop.coordinates.lat;
//    std::cout << std::setprecision(8) << stop.coordinates.lat << std::endl;        // Delete!!!
    input.ignore(1);        // ignore ','
    input >> stop.coordinates.lng;
//    std::cout << std::setprecision(8) << stop.coordinates.lng << std::endl;        // Delete!!!
    return stop;
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
    //std::string_view name = output_request.name;
    //std::cout << name << std::endl;
    return output_request.name;
}
