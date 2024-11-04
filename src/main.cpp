#include "../include/CSVParser.h"
#include "../include/KMeans.h"
#include <iostream>
#include <chrono>
#include <iomanip>

const int CENTROID_COUNT = 20;

int main()
{
    CSVParser parser = CSVParser("data/kmeans_data_set.csv");
    KMeans kmeans(parser.parse_csv(), CENTROID_COUNT);

    auto start = std::chrono::high_resolution_clock::now();
    kmeans.classify_multithreaded_with_simd();
    auto stop = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
    double seconds = duration.count() / 1000.0;
    std::cout << std::fixed << std::setprecision(3) << seconds << " seconds\n";
}
