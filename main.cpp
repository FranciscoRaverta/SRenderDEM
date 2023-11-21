#include "point_io.hpp"
#include "utils.hpp"

#include "vendor/cxxopts.hpp"



int main(int argc, char **argv) {
    cxxopts::Options options("renderdem", "Render a point cloud to a raster DEM");
    options.add_options()
        ("i,input", "Input point cloud (.las, .las, .ply)", cxxopts::value<std::string>())
        ("o,output", "Output GeoTIFF DEM (.tif)", cxxopts::value<std::string>()->default_value("dem.tif"))
        ("t,tile-size", "Tile size", cxxopts::value<int>()->default_value("4096"))
        ("c,classification", "Only use points matching this classification", cxxopts::value<int>()->default_value("-1"))
        ("d,decimation", "Read every Nth point", cxxopts::value<int>()->default_value("1"))
        ("u,output-type", "One of: [min, max, idw]", cxxopts::value<std::string>()->default_value("max"))
        ("r,radiuses", "Comma separated list of radius values to generate and stack", cxxopts::value<std::string>()->default_value("0.56"))
        ("m,max-concurrency", "Maximum number of threads to use", cxxopts::value<int>()->default_value("-1"))
        ("tmpdir", "Temporary directory to store intermediate files", cxxopts::value<std::string>()->default_value("tmp"))
        
        
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
        const auto tileSize = result["tile-size"].as<int>();
        const auto classification = result["classification"].as<int>();
        const auto decimation = result["decimation"].as<int>();
        const auto radiuses = parseCSV(result["radiuses"].as<std::string>());

        for (const double &d: radiuses){
            std::cout << d << std::endl;
        }
        auto *pset = readPointSet(inputFilename, classification, decimation);
        
        
    }
    catch (std::exception &e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
