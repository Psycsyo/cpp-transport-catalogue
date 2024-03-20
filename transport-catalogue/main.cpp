#include "json_reader.h"
#include "request_handler.h"

#include <fstream>

int main() {
    //setlocale(LC_ALL, "rus");
    std::fstream input("input.json");
    if (!input) std::cout << "input error" << std::endl;

    std::fstream output("output.xml");
    if (!output) std::cout << "output error" << std::endl;

    JsonReader json_doc(input);
    input.close();
    transport::Catalogue catalogue;
    json_doc.FillCatalogue(catalogue);

    const auto& stat_requests = json_doc.GetDataRequests();
    const auto& render_settings = json_doc.GetRenderSettings().AsDict();
    const auto& renderer = json_doc.FillRenderSettings(render_settings);

    const auto& routing_settings = json_doc.FillRoutingSettings(json_doc.GetRoutingSettings());
    const transport::Router router = { routing_settings, catalogue };

    RequestHandler request(catalogue, renderer, router);
    json_doc.ProcessRequests(stat_requests, request);

    request.RenderMap().Render(output);
    output.close();
}
