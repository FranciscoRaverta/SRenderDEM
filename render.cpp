#include <filesystem>
#include "render.hpp"

namespace fs = std::filesystem;

void render(PointSet *pset, const std::string &outDir, int tileSize, 
        const std::vector<double> &radiuses, double resolution, bool force){
    fs::path pOutDir = fs::path(outDir);

    if (fs::exists(pOutDir)){
        if (!force) throw std::runtime_error(outDir + " exists (use --force to overwrite results)");
        fs::remove_all(pOutDir);
    }

    fs::create_directories(pOutDir);
}