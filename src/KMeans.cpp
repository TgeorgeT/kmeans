#include "../include/KMeans.h"

#include <utils.h>
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

void KMeans::initCentroids(int centroidCount)
{
    this->centroidCount = centroidCount;
    this->sumX = std::vector<double>(centroidCount);
    this->sumY = std::vector<double>(centroidCount);
    this->count = std::vector<int>(centroidCount);

    if (this->points.size() < centroidCount)
    {
        throw std::runtime_error("Number of points must be at least centroid count.");
    }
    for (int i = 0; i < centroidCount; ++i)
    {
        this->centroids.push_back(this->points[i]);
        this->centroids[i].cluster = i;
    }
}

void KMeans::computeClusters()
{
    size_t pointsSize = this->points.size();
    int shouldIterate = 0;
    for (int i = 0; i < pointsSize; i++)
    {
        double min_dist = DBL_MAX;
        int cluster = -1;

        for (int j = 0; j < this->centroidCount; j++)
        {
            double distance = euclideanDistance(this->points[i], this->centroids[j]);
            double new_min_dist = FindMinimum(min_dist, distance);

            uint64_t mask = -static_cast<int64_t>(new_min_dist == distance);

            min_dist = new_min_dist;
            cluster = (mask & j) | (~mask & cluster);
        }

        shouldIterate |= (cluster != this->points[i].cluster);
        this->points[i].cluster = cluster;

        this->sumX[cluster] += this->points[i].x;
        this->sumY[cluster] += this->points[i].y;
        this->count[cluster]++;
    }
}

void KMeans::classify()
{
    bool shouldIterate = 1;
    double tolerance = 1e-4;

    while (shouldIterate)
    {
        shouldIterate = 0;
        std::fill(this->sumX.begin(), this->sumX.end(), 0);
        std::fill(this->sumY.begin(), this->sumY.end(), 0);
        std::fill(this->count.begin(), this->count.end(), 0);

        this->computeClusters();

        for (int i = 0; i < this->centroidCount; i++)
        {
            Point old_point = this->centroids[i];
            this->centroids[i] = Point{this->sumX[i] / this->count[i], this->sumY[i] / this->count[i], i};

            double shift = euclideanDistance(old_point, this->centroids[i]);

            if (shift > tolerance)
            {
                shouldIterate = 1;
            }
        }
    }
}

void KMeans::writePointsToCSV(const std::string &path)
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