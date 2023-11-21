#include <random>
#include <filesystem>

#include "point_io.hpp"

namespace fs = std::filesystem;

std::string getVertexLine(std::ifstream &reader) {
    std::string line;

    // Skip comments
    do {
        std::getline(reader, line);

        if (line.find("element") == 0)
            return line;
        else if (line.find("comment") == 0)
            continue;
        else
            throw std::runtime_error("Invalid PLY file");
    } while (true);
}

size_t getVertexCount(const std::string &line) {

    // Split line into tokens
    std::vector<std::string> tokens;

    std::istringstream iss(line);
    std::string token;
    while (std::getline(iss, token, ' '))
        tokens.push_back(token);

    if (tokens.size() < 3)
        throw std::runtime_error("Invalid PLY file");

    if (tokens[0] != "element" && tokens[1] != "vertex")
        throw std::runtime_error("Invalid PLY file");

    return std::stoi(tokens[2]);
}

PointSet *readPointSet(const std::string &filename, uint8_t onlyClass) {
    PointSet *r;
    const fs::path p(filename);
    if (p.extension().string() == ".ply"){
        std::cout << std::to_string(onlyClass) << std::endl;
        if (onlyClass != 255) throw std::runtime_error("Classification is not implemented for PLY files.");
        r = fastPlyReadPointSet(filename);
    } else r = pdalReadPointSet(filename, onlyClass);

    return r;
}

PointSet *fastPlyReadPointSet(const std::string &filename) {
    std::ifstream reader(filename, std::ios::binary);
    if (!reader.is_open())
        throw std::runtime_error("Cannot open file " + filename);

    auto *r = new PointSet();

    std::string line;
    std::getline(reader, line);
    line.erase(std::remove(line.begin(), line.end(), '\r'), line.end());

    if (line != "ply")
        throw std::runtime_error("Invalid PLY file (header does not start with ply)");

    std::getline(reader, line);
    line.erase(std::remove(line.begin(), line.end(), '\r'), line.end());

    // We are reading an ascii ply
    bool ascii = line == "format ascii 1.0";

    const auto vertexLine = getVertexLine(reader);
    const auto count = getVertexCount(vertexLine);

    std::cout << "Reading " << count << " points" << std::endl;

    checkHeader(reader, "x");
    checkHeader(reader, "y");
    checkHeader(reader, "z");

    int c = 0;
    bool hasViews = false;
    bool hasNormals = false;
    bool hasColors = false;

    std::getline(reader, line);
    line.erase(std::remove(line.begin(), line.end(), '\r'), line.end());

    while (line != "end_header") {
        if (hasHeader(line, "nx") || hasHeader(line, "normal_x") || hasHeader(line, "normalx")) hasNormals = true;
        if (hasHeader(line, "red") || hasHeader(line, "green") || hasHeader(line, "blue")) hasColors = true;
        if (hasHeader(line, "views")) hasViews = true;

        if (c++ > 100) break;
        std::getline(reader, line);
        line.erase(std::remove(line.begin(), line.end(), '\r'), line.end());
    }

    r->points.resize(count);

    // Read points
    if (ascii) {
        uint16_t buf;
        float fbuf;

        for (size_t i = 0; i < count; i++) {
            reader >> r->points[i][0]
                >> r->points[i][1]
                >> r->points[i][2];
            if (hasNormals) {
                reader >> fbuf
                    >> fbuf
                    >> fbuf;
            }
            if (hasColors) {
                reader >> buf >> buf >> buf;
            }
            if (hasViews) {
                reader >> buf;
            }
        }
    }
    else {

        // Read points
        uint8_t color[3];
        XYZ buf;

        for (size_t i = 0; i < count; i++) {
            reader.read(reinterpret_cast<char *>(&buf), sizeof(float) * 3);
            r->points[i][0] = static_cast<double>(buf.x);
            r->points[i][1] = static_cast<double>(buf.y);
            r->points[i][2] = static_cast<double>(buf.z);
            
            if (hasNormals) {
                reader.read(reinterpret_cast<char *>(&buf), sizeof(float) * 3);
            }

            if (hasColors) {
                reader.read(reinterpret_cast<char *>(&color), sizeof(uint8_t) * 3);
            }

            if (hasViews) {
                reader.read(reinterpret_cast<char *>(&color[0]), sizeof(uint8_t));
            }
        }
    }

    // for (size_t idx = 0; idx < count; idx++) {
    //     std::cout << r->points[idx][0] << " ";
    //     std::cout << r->points[idx][1] << " ";
    //     std::cout << r->points[idx][2] << " ";

    //     std::cout << std::endl;

    //     if (idx > 9) exit(1);
    // }

    // exit(1);

    reader.close();

    return r;
}

PointSet *pdalReadPointSet(const std::string &filename, uint8_t onlyClass) {
    #ifdef WITH_PDAL
    std::string classDimension;
    pdal::StageFactory factory;
    const std::string driver = pdal::StageFactory::inferReaderDriver(filename);
    if (driver.empty()) {
        throw std::runtime_error("Can't infer point cloud reader from " + filename);
    }

    auto *r = new PointSet();
    pdal::Stage *s = factory.createStage(driver);
    pdal::Options opts;
    opts.add("filename", filename);
    s->setOptions(opts);

    auto *table = new pdal::PointTable();

    std::cout << "Reading points from " << filename << std::endl;

    s->prepare(*table);
    const pdal::PointViewSet pvSet = s->execute(*table);

    r->pointView = *pvSet.begin();
    const pdal::PointViewPtr pView = r->pointView;

    if (pView->empty()) {
        throw std::runtime_error("No points could be fetched");
    }

    std::cout << "Number of points: " << pView->size() << std::endl;

    for (const auto &d : pView->dims()) {
        std::string dim = pView->dimName(d);
        if (dim == "Label" || dim == "label" ||
            dim == "Classification" || dim == "classification" ||
            dim == "Class" || dim == "class") {
            classDimension = dim;
        }
    }

    const size_t count = pView->size();
    const pdal::PointLayoutPtr layout(table->layout());
    const bool hasClass = !classDimension.empty();

    pdal::Dimension::Id classId;
    if (hasClass) {
        std::cout << "Classification dimension: " << classDimension << std::endl;
        classId = layout->findDim(classDimension);
    }

    r->points.resize(count);
    if (!hasClass && onlyClass != 255) throw std::runtime_error("Cannot filter by classification (no classification dimension found)");
    bool filter = hasClass && onlyClass != 255;

    pdal::PointId i = 0;
    for (pdal::PointId idx = 0; idx < count; ++idx) {
        auto p = pView->point(idx);
        if (filter && p.getFieldAs<uint8_t>(classId) != onlyClass) continue; // Skip

        r->points[i][0] = p.getFieldAs<double>(pdal::Dimension::Id::X);
        r->points[i][1] = p.getFieldAs<double>(pdal::Dimension::Id::Y);
        r->points[i][2] = p.getFieldAs<double>(pdal::Dimension::Id::Z);
        i++;
    }

    // for (size_t idx = 0; idx < count; idx++) {
    //     std::cout << r->points[idx][0] << " ";
    //     std::cout << r->points[idx][1] << " ";
    //     std::cout << r->points[idx][2] << " ";

    //     std::cout << std::endl;

    //     if (idx > 9) exit(1);
    // }

    // exit(1);

    return r;
    #else
    fs::path p(filename);
    throw std::runtime_error("Unsupported file extension " + p.extension().string() + ", build program with PDAL support for additional file types support.");
    #endif
}

void checkHeader(std::ifstream &reader, const std::string &prop) {
    std::string line;
    std::getline(reader, line);
    line.erase(std::remove(line.begin(), line.end(), '\r'), line.end());

    if (line.substr(line.length() - prop.length(), prop.length()) != prop) {
        throw std::runtime_error("Invalid PLY file (expected 'property * " + prop + "', but found '" + line + "')");
    }
}

bool hasHeader(const std::string &line, const std::string &prop) {
    //std::cout << line << " -> " << prop << " : " << line.substr(line.length() - prop.length(), prop.length()) << std::endl;
    return line.substr(0, 8) == "property" && line.substr(line.length() - prop.length(), prop.length()) == prop;
}

bool fileExists(const std::string &path) {
    std::ifstream fin(path);
    const bool e = fin.good();
    fin.close();
    return e;
}
