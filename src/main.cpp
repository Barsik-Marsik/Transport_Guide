#include "input_reader.h"
#include "log_duration.h"
#include "stat_reader.h"
//#include "tests.h"

#include <cassert>
#include <fstream>
#include <iostream>


void MainTest() {
    std::ifstream input("tests/tsC_case1_input.txt");
    std::ofstream output("tests/tsC_case1_output_result.txt");
//    std::istream input = std::cin;
//    std::ostream output = std::cout;
    int N;
    input >> N;
    input.ignore(1);
    TransportCatalogue transport_catalogue;
    RequestQueue request_queue(transport_catalogue);
    // Writing input requests to the request queue
    request_queue.ReadRawRequests(input, N);
//    std::cout << "Writing input requests to the request queue is OK" <<std::endl;
    // Writing information about stops and buses to the Transport cataloque
    request_queue.ProcessInputRequests();
//    std::cout << "Writing information about stops and buses to the Transport cataloque is OK" <<std::endl;
    // Reading output requests and writing result to the outstream
    input >> N;
    input.ignore(1);
    request_queue.ReadRawRequests(input, N);
//    std::cout << "Reading output requests and writing result to the outstream is OK" <<std::endl;
    // Getting info about the bus
    request_queue.ProcessOutputRequests(output);
//    std::cout << "Getting info about the bus is OK" <<std::endl;
}

std::ostream& operator<<(std::ostream& output, const RequestType& type) {
	if (type == RequestType::Bus_r) {
		output << "Bus_r";
		return output;
	}
	if (type == RequestType::Bus_w) {
		output << "Bus_w";
		return output;
	}
	output << "Stop_w";
	return output;
}

int main() {

    MainTest();
//    TestParseRawRequest();
//    TestParseStopRequest();
//    TestParseBusRequest();

//    std::cout << "OK";

    return 0;
}
