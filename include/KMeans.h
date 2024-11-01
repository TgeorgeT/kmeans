#pragma once
#include <vector>
#include <atomic>
#include <string>
#include <mutex>
#include "types.h"

class KMeans
{
private:
    std::vector<Point> points;
    std::vector<Point> centroids;

    std::vector<double> sum_x, sum_y;
    std::vector<size_t> count;

    std::mutex mu;
    int centroid_count;

public:
    KMeans(const std::vector<std::pair<double, double>>);
    void init_centroids(int);
    void compute_clusters();
    void compute_clusters_parallel();
    void classify();
    void classify_parallel();
    void write_points_to_csv(const std::string &);
};
