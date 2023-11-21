#include "point_io.hpp"

#include "vendor/cxxopts.hpp"



int main(int argc, char **argv) {
    cxxopts::Options options("renderdem", "Render a point cloud to a raster DEM");
    options.add_options()
        ("i,input", "Input point cloud (.las, .las, .ply)", cxxopts::value<std::string>())
        ("o,output", "Output GeoTIFF DEM (.tif)", cxxopts::value<std::string>()->default_value("dem.tif"))
        ("t,tilewidth", "Tile width", cxxopts::value<int>()->default_value("4096"))
        ("h,help", "Print usage")
        ;
    options.parse_positional({ "input" });
    options.positional_help("[point cloud]");
    cxxopts::ParseResult result;
    try {
        result = options.parse(argc, argv);
    }
    catch (const std::exception &e) {
        std::cerr << e.what() << std::endl;
        std::cerr << options.help() << std::endl;
        return EXIT_FAILURE;
    }

    if (result.count("help") || !result.count("input")) {
        std::cout << options.help() << std::endl;
        return EXIT_SUCCESS;
    }

    try {
        const auto inputFilename = result["input"].as<std::string>();
        const auto outputFilename = result["output"].as<std::string>();
        const auto tileWidth = result["tilewidth"].as<int>();

        auto *pset = readPointSet(inputFilename);
    }
    catch (std::exception &e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
