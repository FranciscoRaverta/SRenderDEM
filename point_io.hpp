#ifndef POINTIO_H
#define POINTIO_H

#include <iostream>
#include <fstream>
#include <vector>
#include <array>
#include <algorithm>

#ifdef WITH_PDAL
#include <pdal/Options.hpp>
#include <pdal/PointTable.hpp>
#include <pdal/StageFactory.hpp>
#include <pdal/io/BufferReader.hpp>
#endif

struct XYZ {
    float x;
    float y;
    float z;
};

struct PointSet {
    std::vector<std::array<double, 3> > points;

    #ifdef WITH_PDAL
    pdal::PointViewPtr pointView = nullptr;
    #endif

    inline size_t count() const { return points.size(); }

    void appendPoint(PointSet &src, size_t idx) {
        points.push_back(src.points[idx]);
    }

    ~PointSet() {
    }
};

std::string getVertexLine(std::ifstream &reader);
size_t getVertexCount(const std::string &line);
inline void checkHeader(std::ifstream &reader, const std::string &prop);
inline bool hasHeader(const std::string &line, const std::string &prop);

PointSet *fastPlyReadPointSet(const std::string &filename);
PointSet *pdalReadPointSet(const std::string &filename, uint8_t onlyClass = 255);
PointSet *readPointSet(const std::string &filename, uint8_t onlyClass = 255);

bool fileExists(const std::string &path);


#endif
