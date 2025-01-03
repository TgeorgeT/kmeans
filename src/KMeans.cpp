#include "../include/KMeans.h"

#include <stdio.h>
#include <string.h>
#include <utils.h>
#include <thread>
#include <atomic>
#include <fstream>
#include <string>
#include <iostream>
#include <float.h>
#include <stdexcept>
#include <algorithm>
#include <immintrin.h>
#include <cstdio>
#include <cstdlib>

KMeans::KMeans(const std::vector<std::pair<double, double>> points, size_t centroid_count)
{
    this->points_size = points.size();
    this->centroid_count = centroid_count;

    if (this->points_size < centroid_count)
    {
        throw std::runtime_error("Number of points must be at least centroid count.");
    }

    this->points_x = static_cast<double *>(std::aligned_alloc(32, points.size() * sizeof(double)));
    this->points_y = static_cast<double *>(std::aligned_alloc(32, points.size() * sizeof(double)));
    this->assigned_cluster = new int32_t[this->points_size];

    for (size_t i = 0; i < this->points_size; ++i)
    {
        this->points_x[i] = points[i].first;
        this->points_y[i] = points[i].second;
        this->assigned_cluster[i] = -1;
    }

    this->centroids_x = static_cast<double *>(std::aligned_alloc(32, centroid_count * sizeof(double)));
    this->centroids_y = static_cast<double *>(std::aligned_alloc(32, centroid_count * sizeof(double)));
    this->sum_x = new double[centroid_count];
    this->sum_y = new double[centroid_count];
    this->count = new size_t[centroid_count];

    for (int i = 0; i < centroid_count; ++i)
    {
        this->centroids_x[i] = this->points_x[i];
        this->centroids_y[i] = this->points_y[i];
    }
}

KMeans::~KMeans()
{
    delete[] this->sum_x;
    delete[] this->sum_y;
    delete[] this->points_x;
    delete[] this->points_y;
    delete[] this->centroids_x;
    delete[] this->centroids_y;
    delete[] this->assigned_cluster;
}

void KMeans::compute_clusters()
{
    for (size_t i = 0; i < this->points_size; i++)
    {
        double min_dist = DBL_MAX;
        int cluster = -1;

        for (int j = 0; j < this->centroid_count; j++)
        {
            double distance = euclidean_distance(this->points_x[i], this->points_y[i], this->centroids_x[j], this->centroids_y[j]);
            double new_min_dist = find_minimum(min_dist, distance);

            uint64_t mask = -static_cast<int64_t>(new_min_dist == distance);

            min_dist = new_min_dist;
            cluster = (mask & j) | (~mask & cluster);
        }

        this->assigned_cluster[i] = cluster;
        this->sum_x[cluster] += this->points_x[i];
        this->sum_y[cluster] += this->points_y[i];
        this->count[cluster]++;
    }
}

void KMeans::classify()
{
    bool should_iterate = 1;
    double tolerance = 1e-4;

    while (should_iterate)
    {
        should_iterate = 0;
        std::fill(this->sum_x, this->sum_x + this->centroid_count, 0);
        std::fill(this->sum_y, this->sum_y + this->centroid_count, 0);
        std::fill(this->count, this->count + this->centroid_count, 0);

        this->compute_clusters();

        for (int i = 0; i < this->centroid_count; i++)
        {
            double old_point_x = this->centroids_x[i], old_point_y = this->centroids_y[i];
            this->centroids_x[i] = this->sum_x[i] / this->count[i];
            this->centroids_y[i] = this->sum_y[i] / this->count[i];

            double shift = euclidean_distance(old_point_x, old_point_y, this->centroids_x[i], this->centroids_y[i]);

            if (shift > tolerance)
            {
                should_iterate = 1;
            }
        }
    }
}

void KMeans::compute_clusters_multithreaded()
{
    unsigned int thread_count = std::thread::hardware_concurrency();
    unsigned int chunk_size = this->points_size / thread_count;

    std::vector<std::thread> threads;

    for (size_t i = 0; i < this->points_size; i += chunk_size)
    {
        threads.push_back(
            std::thread([this, i, chunk_size]()
                        {
            std::vector<double> local_sum_x = std::vector<double>(centroid_count), local_sum_y = std::vector<double>(centroid_count);
            std::vector<size_t> local_count = std::vector<size_t>(centroid_count);
            size_t j = find_minimum(i + chunk_size, points_size);

            for (size_t k = i; k < j; k++)
            {
                double min_dist = DBL_MAX;
                int cluster = -1;

                for (int j = 0; j < this->centroid_count; j++)
                {
                    double distance = euclidean_distance(this->points_x[k], this->points_y[k], this->centroids_x[j], this->centroids_y[j]);
                    double new_min_dist = find_minimum(min_dist, distance);

                    uint64_t mask = -static_cast<int64_t>(new_min_dist == distance);

                    min_dist = new_min_dist;
                    cluster = (mask & j) | (~mask & cluster);
                }
                this->assigned_cluster[k]=cluster;

                local_sum_x[cluster] += this->points_x[k];
                local_sum_y[cluster] += this->points_y[k];
                local_count[cluster] += 1;
            }
            {
                std::lock_guard<std::mutex> lock(this->mu);
                for (int j = 0; j < this->centroid_count; j++)
                {
                    this->sum_x[j] += local_sum_x[j];
                    this->sum_y[j] += local_sum_y[j];
                    this->count[j] += local_count[j];
                }
            } }));
    }

    for (std::thread &thread : threads)
    {
        if (thread.joinable())
        {
            thread.join();
        }
        else
        {
            std::cerr << "Non joinable thread\n";
        }
    }
}

void KMeans::classify_multithreaded()
{
    bool should_iterate = 1;
    double tolerance = 1e-4;

    while (should_iterate)
    {
        should_iterate = 0;
        std::fill(this->sum_x, this->sum_x + this->centroid_count, 0);
        std::fill(this->sum_y, this->sum_y + this->centroid_count, 0);
        std::fill(this->count, this->count + this->centroid_count, 0);

        this->compute_clusters_multithreaded();

        for (int i = 0; i < this->centroid_count; i++)
        {
            double old_point_x = this->centroids_x[i], old_point_y = this->centroids_y[i];
            this->centroids_x[i] = this->sum_x[i] / this->count[i];
            this->centroids_y[i] = this->sum_y[i] / this->count[i];

            double shift = euclidean_distance(old_point_x, old_point_y, this->centroids_x[i], this->centroids_y[i]);

            if (shift > tolerance)
            {
                should_iterate = 1;
            }
        }
    }
}

void KMeans::compute_clusters_multithreaded_with_simd()
{
    bool should_iterate = 0;

    unsigned int thread_count = std::thread::hardware_concurrency();
    unsigned int chunk_size = points_size / thread_count;

    std::vector<std::thread> threads;

    for (size_t i = 0; i < this->points_size; i += chunk_size)
    {
        threads.push_back(
            std::thread([this, i, &should_iterate, chunk_size]()
                        {
                std::vector<double> local_sum_x(this->centroid_count, 0.0);
                std::vector<double> local_sum_y(this->centroid_count, 0.0);
                std::vector<size_t> local_count(this->centroid_count, 0);

                size_t j = find_minimum(i + chunk_size, points_size);

                int max_chunk = 4 * (this->centroid_count / 4);

                for (size_t k = i; k < j; k++) {
                    __m256d point_x = _mm256_set1_pd(points_x[k]);
                    __m256d point_y = _mm256_set1_pd(points_y[k]);
                    int cluster = -1;
                    double min_dist = DBL_MAX;

                    int j;
                    for (j = 0; j < max_chunk; j += 4) {
                        __m256d centroid_x = _mm256_loadu_pd(&(this->centroids_x[j]));
                        __m256d centroid_y = _mm256_loadu_pd(&(this->centroids_y[j]));

                        __m256d dx = _mm256_sub_pd(point_x, centroid_x);
                        __m256d dy = _mm256_sub_pd(point_y, centroid_y);
                        __m256d dist_squared = _mm256_add_pd(_mm256_mul_pd(dx, dx), _mm256_mul_pd(dy, dy));

                        __m256d t0 = _mm256_permute2f128_pd(dist_squared, dist_squared, 0x01);
                        t0 = _mm256_min_pd(t0, dist_squared);
                        __m256d t1 = _mm256_shuffle_pd(t0, t0, 0b0101);
                        t0 = _mm256_min_pd(t0, t1);

                        double new_potential_min = _mm256_cvtsd_f64(t0);

                        uint32_t closest_centroid_mask = _mm256_movemask_pd(_mm256_cmp_pd(dist_squared, t0, _CMP_EQ_OQ));
                        uint32_t position = __builtin_ctz(closest_centroid_mask);

                        min_dist = find_minimum(new_potential_min, min_dist);
                        uint64_t min_mask = -static_cast<int64_t>(new_potential_min == min_dist);
                        cluster = (min_mask & (j + position)) | (~min_mask & cluster);
                    }

                    for (; j < this->centroid_count; ++j) {
                        double new_potential_min = euclidean_distance(points_x[k], points_y[k], centroids_x[j], centroids_y[j]);
                        min_dist = find_minimum(min_dist, new_potential_min);

                        uint64_t mask = -static_cast<int64_t>(new_potential_min == min_dist);
                        cluster = (mask & j) | (~mask & cluster);
                    }

                    this->assigned_cluster[k] = cluster;
                    local_sum_x[cluster] += this->points_x[k];
                    local_sum_y[cluster] += this->points_y[k];
                    local_count[cluster]++;
                }

                {
                    std::lock_guard<std::mutex> lock(this->mu);
                    for (int j = 0; j < this->centroid_count; ++j) {
                        this->sum_x[j] += local_sum_x[j];
                        this->sum_y[j] += local_sum_y[j];
                        this->count[j] += local_count[j];
                    }
                } }));
    }

    for (std::thread &thread : threads)
    {
        if (thread.joinable())
        {
            thread.join();
        }
        else
        {
            std::cerr << "Non-joinable thread\n";
        }
    }
}

void KMeans::classify_multithreaded_with_simd()
{
    bool should_iterate = 1;
    double tolerance = 1e-4;

    while (should_iterate)
    {
        should_iterate = 0;
        std::fill(this->sum_x, this->sum_x + this->centroid_count, 0);
        std::fill(this->sum_y, this->sum_y + this->centroid_count, 0);
        std::fill(this->count, this->count + this->centroid_count, 0);

        this->compute_clusters_multithreaded_with_simd();

        for (int i = 0; i < this->centroid_count; i++)
        {
            double old_point_x = this->centroids_x[i], old_point_y = this->centroids_y[i];
            this->centroids_x[i] = this->sum_x[i] / this->count[i];
            this->centroids_y[i] = this->sum_y[i] / this->count[i];

            double shift = euclidean_distance(old_point_x, old_point_y, this->centroids_x[i], this->centroids_y[i]);

            if (shift > tolerance)
            {
                should_iterate = 1;
            }
        }
    }
}

void KMeans::write_points_to_csv(const std::string &path)
{
    std::ofstream file(path);

    if (file.fail())
    {
        std::cerr << "Error: Failed to open file " << path << std::endl;
        return;
    }

    file << "X,Y,cluster\n";

    for (size_t i = 0; i < this->points_size; ++i)
    {
        file << this->points_x[i] << "," << this->points_y[i] << "," << this->assigned_cluster[i] << "\n";
    }

    file.close();
}
