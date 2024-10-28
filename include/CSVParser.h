#pragma once
#include <vector>
#include <string>

class CSVParser
{
private:
    std::string path;

public:
    CSVParser(const std::string &path) : path(path) {}
    std::vector<std::pair<double, double>> parseCSV();
};