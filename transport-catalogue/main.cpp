#include "json_reader.h"
#include "request_handler.h"

#include <fstream>

int main() {
    setlocale(LC_ALL, "rus");
    std::fstream input("input.json");
    if (!input) std::cout << "input error" << std::endl;

    std::fstream output("output.xml");
    if (!output) std::cout << "output error" << std::endl;

    JsonReader json_doc(input);
    transport::Catalogue catalogue;
    json_doc.FillCatalogue(catalogue);

    const auto& stat_requests = json_doc.GetDataRequests();
    const auto& render_settings = json_doc.GetRenderSettings().AsMap();
    const auto& renderer = json_doc.FillRenderSettings(render_settings);

    RequestHandler request(catalogue, renderer);
    json_doc.ProcessRequests(stat_requests, request);
    request.RenderMap().Render(output);

    input.close();
    output.close();
}
