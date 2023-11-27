#ifndef RENDER_H
#define RENDER_H

#include <vector>

#include "point_io.hpp"

void render(PointSet *pset, const std::string &outDir, const std::string &outputType,
        int tileSize, 
        const std::vector<double> &radiuses, double resolution, 
        int maxTiles, bool force);


#endif