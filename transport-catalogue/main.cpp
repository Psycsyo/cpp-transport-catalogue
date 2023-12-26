#include "input_reader.h"
#include "stat_reader.h"

using namespace Transport;

int main() {
    Catalogue catalogue;
    FillCatalogue(std::cin, catalogue);
    ProcessRequests(std::cin, catalogue, std::cout);
}
