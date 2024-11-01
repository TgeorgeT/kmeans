#include "../include/CSVParser.h"
#include "../include/KMeans.h"
#include <iostream>
#include <chrono>
#include <iomanip>

const int CENTROID_COUNT = 5;

int main()
{
    CSVParser parser = CSVParser("data/kmeans_data_set.csv");
    KMeans kmeans(parser.parse_csv());
    kmeans.init_centroids(CENTROID_COUNT);

    auto start = std::chrono::high_resolution_clock::now();
    kmeans.classify_parallel();
    auto stop = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
    double seconds = duration.count() / 1000.0;
    std::cout << std::fixed << std::setprecision(3) << seconds << " seconds\n";

    kmeans.write_points_to_csv("data/kmeans_result.csv");
}
