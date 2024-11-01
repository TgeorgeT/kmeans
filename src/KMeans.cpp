#include "../include/KMeans.h"

#include <utils.h>
#include <thread>
#include <atomic>
#include <fstream>
#include <string>
#include <iostream>
#include <float.h>
#include <stdexcept>
#include <algorithm>

#include "../include/types.h"

KMeans::KMeans(const std::vector<std::pair<double, double>> points)
{
    for (const std::pair<double, double> &point : points)
    {
        this->points.push_back({point.first, point.second, -1});
    }
}

void KMeans::init_centroids(int centroid_count)
{
    this->centroid_count = centroid_count;
    this->sum_x = std::vector<double>(centroid_count);
    this->sum_y = std::vector<double>(centroid_count);
    this->count = std::vector<size_t>(centroid_count);

    if (this->points.size() < centroid_count)
    {
        throw std::runtime_error("Number of points must be at least centroid count.");
    }
    for (int i = 0; i < centroid_count; ++i)
    {
        this->centroids.push_back(this->points[i]);
        this->centroids[i].cluster = i;
    }
}

void KMeans::compute_clusters()
{
    size_t points_size = this->points.size();
    bool should_iterate = 0;
    for (size_t i = 0; i < points_size; i++)
    {
        double min_dist = DBL_MAX;
        int cluster = -1;

        for (int j = 0; j < this->centroid_count; j++)
        {
            double distance = euclidean_distance(this->points[i], this->centroids[j]);
            double new_min_dist = std::min(min_dist, distance);

            uint64_t mask = -static_cast<int64_t>(new_min_dist == distance);

            min_dist = new_min_dist;
            cluster = (mask & j) | (~mask & cluster);
        }

        should_iterate |= (cluster != this->points[i].cluster);
        this->points[i].cluster = cluster;

        this->sum_x[cluster] += this->points[i].x;
        this->sum_y[cluster] += this->points[i].y;
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
        std::fill(this->sum_x.begin(), this->sum_x.end(), 0);
        std::fill(this->sum_y.begin(), this->sum_y.end(), 0);
        std::fill(this->count.begin(), this->count.end(), 0);

        this->compute_clusters();

        for (int i = 0; i < this->centroid_count; i++)
        {
            Point old_point = this->centroids[i];
            this->centroids[i] = Point{this->sum_x[i] / this->count[i], this->sum_y[i] / this->count[i], i};

            double shift = euclidean_distance(old_point, this->centroids[i]);

            if (shift > tolerance)
            {
                should_iterate = 1;
            }
        }
    }
}

void KMeans::compute_clusters_parallel()
{
    size_t points_size = this->points.size();
    bool should_iterate = 0;

    unsigned int thread_count = std::thread::hardware_concurrency();
    unsigned int chunk_size = points_size / thread_count;

    std::vector<std::thread> threads;

    for (size_t i = 0; i < points_size; i += chunk_size)
    {
        threads.push_back(
            std::thread([this, i, &should_iterate, chunk_size, points_size]()
                        {
            bool local_should_iterate = 0;
            std::vector<double> local_sum_x = std::vector<double>(centroid_count), local_sum_y = std::vector<double>(centroid_count);
            std::vector<size_t> local_count = std::vector<size_t>(centroid_count);
            size_t end = std::min(i + chunk_size, points_size);

            for (size_t k = i; k < end; k++)
            {
                double min_dist = DBL_MAX;
                int cluster = -1;

                for (int j = 0; j < this->centroid_count; j++)
                {
                    double distance = euclidean_distance(this->points[k], this->centroids[j]);
                    double new_min_dist = std::min(min_dist, distance);

                    uint64_t mask = -static_cast<int64_t>(new_min_dist == distance);
                    min_dist = new_min_dist;
                    cluster = (mask & j) | (~mask & cluster);
                }

                local_should_iterate |= (cluster != this->points[k].cluster);
                this->points[k].cluster = cluster;

                local_sum_x[cluster] += this->points[k].x;
                local_sum_y[cluster] += this->points[k].y;
                local_count[cluster] += 1;


            }
            {
                std::lock_guard<std::mutex> lock(this->mu);
                should_iterate |= local_should_iterate;
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

void KMeans::classify_parallel()
{
    bool should_iterate = 1;
    double tolerance = 1e-4;

    while (should_iterate)
    {
        should_iterate = 0;
        std::fill(this->sum_x.begin(), this->sum_x.end(), 0);
        std::fill(this->sum_y.begin(), this->sum_y.end(), 0);
        std::fill(this->count.begin(), this->count.end(), 0);

        this->compute_clusters_parallel();

        for (int i = 0; i < this->centroid_count; i++)
        {
            Point old_point = this->centroids[i];
            this->centroids[i] = Point{this->sum_x[i] / this->count[i], this->sum_y[i] / this->count[i], i};

            double shift = euclidean_distance(old_point, this->centroids[i]);

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

    for (const Point &point : this->points)
    {
        file << point.x << "," << point.y << "," << point.cluster << "\n";
    }

    file.close();
}
