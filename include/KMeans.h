#pragma once
#include <vector>
#include "types.h"

class KMeans
{
private:
    std::vector<Point> points;
    std::vector<Point> centroids;
    std::vector<double> sumX, sumY;
    std::vector<int> count;
    int centroidCount;

    void assignClusters();

public:
    KMeans(const std::vector<std::pair<double, double>>);
    void initCentroids(int);
    void assignClusters(int);
};