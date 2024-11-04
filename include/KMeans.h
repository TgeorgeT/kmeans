#pragma once
#include <vector>
#include <atomic>
#include <string>
#include <mutex>

class KMeans
{
private:
    double *sum_x, *sum_y;
    size_t *count, points_size, centroid_count;

    double *points_x, *points_y, *centroids_x, *centroids_y;
    int32_t *assigned_cluster;

    std::mutex mu;

public:
    KMeans(const std::vector<std::pair<double, double>>, size_t);
    ~KMeans();
    void compute_clusters();
    void classify();
    void compute_clusters_multithreaded();
    void classify_multithreaded();
    void compute_clusters_multithreaded_with_simd();
    void classify_multithreaded_with_simd();
    void write_points_to_csv(const std::string &);
};
