
#include "../include/CSVParser.h"

#include <vector>
#include <string>
#include <fstream>
#include <iostream>
#include <sstream>
std::vector<std::pair<double, double>> CSVParser::parseCSV()
{
    std::vector<std::pair<double, double>> points;
    std::ifstream file(this->path);

    if (file.fail())
    {
        std::cerr << "Error: Failed to open file " << path << std::endl;
        return points;
    }

    std::string line;

    double x, y;
    char comma;
    bool first = 1;
    while (std::getline(file, line))
    {
        if (first == true)
        {
            first = false;
            continue;
        }

        std::istringstream lineStream(line);

        lineStream >> x >> comma >> y;
        if (lineStream.fail())
        {
            std::cerr << "Failed reading line: " << line << "\n";
            lineStream.clear();
        }
        points.emplace_back(x, y);
    }
    file.close();
    return points;
};