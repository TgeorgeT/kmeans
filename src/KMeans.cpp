#include "../include/KMeans.h"

#include <stdexcept>

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
        throw std::runtime_error("Number of points must be at least 5.");
    }
    for (int i = 0; i < centroidCount; ++i)
    {
        this->centroids.push_back(this->points[i]);
        this->centroids[i].cluster = i;
    }
}

void KMeans::assignClusters()
{
    size_t pointsSize = this->points.size();
    for (int i = 0; i < pointsSize; i++)
    {
        double min_dist;
        int cluster;

        for (int i = 0; i < centroidCount; i++)
        {
        }
    }
}