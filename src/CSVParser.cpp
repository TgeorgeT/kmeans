#include "../include/CSVParser.h"

#include <vector>
#include <string>
#include <fstream>
#include <iostream>
#include <sstream>

std::vector<std::pair<double, double>> CSVParser::parse_csv()
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
    bool first = true;

    while (std::getline(file, line))
    {
        if (first)
        {
            first = false;
            continue;
        }

        std::istringstream line_stream(line);
        line_stream >> x >> comma >> y;

        if (line_stream.fail())
        {
            std::cerr << "Failed reading line: " << line << "\n";
            line_stream.clear();
        }
        points.emplace_back(x, y);
    }

    file.close();
    return points;
}
