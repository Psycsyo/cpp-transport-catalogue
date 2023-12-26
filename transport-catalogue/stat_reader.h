#pragma once

#include "transport_catalogue.h"

#include <iostream>
namespace Transport {
    
void ProcessRequests(std::istream& in, Catalogue& catalogue, std::ostream& out);
void PrintBusData(std::string& line, Catalogue& catalogue, std::ostream& out);
void PrintStopData(std::string& line, Catalogue& catalogue, std::ostream& out);  
    
}
