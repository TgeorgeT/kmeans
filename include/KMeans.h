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

    double *sum_x, *sum_y;
    size_t *count, points_size;

    double *points_x, *points_y, *centroids_x, *centroids_y;
    int32_t *assigned_cluster;

    std::mutex mu;
    int centroid_count;

public:
    KMeans(const std::vector<std::pair<double, double>>);
    void init_centroids(int);
    void compute_clusters();
    void compute_clusters_parallel();
    void classify();
    void classify_parallel();
    void compute_clusters_parallel_with_simd();
    void classify_parallel_with_simd();
    void write_points_to_csv(const std::string &);
};
