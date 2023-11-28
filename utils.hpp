#ifndef UTILS_H
#define UTILS_H

#ifdef WIN32
#ifndef NOMINMAX
# define NOMINMAX
#endif
#include <windows.h>
#else
#include <unistd.h>
#endif

static inline std::vector<std::string> split(const std::string &s, const std::string &delimiter){
    size_t posStart = 0, posEnd, delimLen = delimiter.length();
    std::string token;
    std::vector<std::string> res;

    while ((posEnd = s.find(delimiter, posStart)) != std::string::npos) {
        token = s.substr(posStart, posEnd - posStart);
        posStart = posEnd + delimLen;
        res.push_back(token);
    }

    res.push_back(s.substr(posStart));
    return res;
}

static inline std::vector<double> parseCSV(const std::string &s){
    std::vector<double> res;
    const auto values = split(s, ",");
    for (const std::string &v : values){
        res.push_back(std::stod(v));
    }
    return res;
}


#ifdef WIN32
static unsigned long long getTotalMemory(){
    MEMORYSTATUSEX status;
    status.dwLength = sizeof(status);
    GlobalMemoryStatusEx(&status);
    return status.ullTotalPhys;
}
#else
static unsigned long long getTotalMemory(){
    long pages = sysconf(_SC_PHYS_PAGES);
    long page_size = sysconf(_SC_PAGE_SIZE);
    return pages * page_size;
}
#endif

#endif