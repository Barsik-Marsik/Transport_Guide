#include "log_duration.h"
#include "tests.h"

#include <cassert>
#include <string>
#include <sstream>
#include <vector>


void TestParseRawRequest() {
    TransportCatalog tc;
    RequestQueue rq(tc);
    {
		std::string text = "Stop Tol okoncevo: 55.673512, 41.021445";
		std::istringstream input(text);
		rq.ReadRawRequests(input, 1);
		rq.PrintAllRequests(std::cout);
    }
    {
		std::string text = "Bus 7A:  Tolokoncevo  >   Spartak   >  CSKA ";
		std::istringstream input(text);
		rq.ReadRawRequests(input, 1);
		rq.PrintAllRequests(std::cout);
    }
    {
		std::string text = "Bus 303E";
		std::istringstream input(text);
		rq.ReadRawRequests(input, 1);
		rq.PrintAllRequests(std::cout);
    }
}

void TestParseStopRequest() {

}

void TestParseBusRequest() {
    TransportCatalog tc;
    RequestQueue rq(tc);
    {
		Request request = {RequestType::Bus_w, "7A", "  Tolo koncevo > LOKO >   Spartak   >  SKA"};
		std::string text = request.text;
		std::pair<std::string, std::vector<std::string>> bus = {"7A", {"Tolo koncevo", "Spartak", "SKA", "Tolokoncevo"}};
		std::cout << "request.text.substr(2, 12): '" << request.text.substr(2, 12) << "'" << std::endl;
		//assert(bus == rq.ParseBusRequest(request));
		auto result = rq.ParseBusRequest(request);
		std::cout << "result.first '" << result.first << "' "
				<< "result.second[0] '" << result.second[0]
				<< std::endl;
		assert(bus.first == result.first);
		std::cout << text.substr(2, 12) << "!!!!\n" << std::endl;
		assert("Tolo koncevo" == result.second[0]);
		assert("LOKO" == result.second[1]);
		assert("Spartak" == result.second[2]);
		assert("SKA" == result.second[3]);
		assert(">" == result.second[4]);
    }
}
