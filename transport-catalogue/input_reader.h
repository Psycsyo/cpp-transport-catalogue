#pragma once

#include "transport_catalogue.h"

#include <iostream>

namespace Transport {
    
void FillCatalogue(std::istream& in, Catalogue& catalogue);
std::pair<std::vector<std::string>, bool> FillRoute(std::string& line);
    
}
