#pragma once

#include "geo.h"
#include "transport_catalogue.h"

#include <deque>
#include <iostream>
#include <string>
#include <sstream>
#include <vector>


enum RequestType {
    Bus_r,      // reading info about the bus from catalog
    Stop_r,      // reading info about the stop from catalog
    Bus_w,      // writing info about the bus to catalogue
    Stop_w,     // writing info about the stop to catalogue
};

struct Request {
    RequestType type;
    std::string name;
    std::string text;
};

std::ostream& operator<<(std::ostream& output, const RequestType& type);

std::ostream& operator<<(std::ostream& output, const Request& request);

std::string ReadLine(std::istream& input);

class RequestQueue {
public:
    explicit RequestQueue(TransportCatalogue& transport_catalogue);

    void ReadRawRequests(std::istream& input, int num);

    void AddRequest(std::string raw_string);

    void ProcessInputRequests();

    void ProcessOutputRequests(std::ostream& output);

    void PrintAllRequests(std::ostream& output);

private:
    TransportCatalogue& transport_catalogue_;
    std::deque<Request> input_requests_;
    std::deque<Request> output_requests_;
    std::deque<std::pair<std::string, std::string>> raw_distances_;
public:
    Request ParseRawRequest(std::string text);

    void RemoveSpace(std::string& str);

    Stop ParseStopRequest(Request& stop_request);

    std::pair<std::string, std::vector<std::string>> ParseBusRequest(Request& bus_request);

    std::vector<std::pair<std::string, int>> ParseDistances(std::string raw_distance);

    std::string_view ParseOutputRequest(const Request& output_request);
};
