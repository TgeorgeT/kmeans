#include "../include/CSVParser.h"
#include "../include/KMeans.h"
#include <iostream>

const int CENTROID_COUNT = 5;

int main()
{
    CSVParser parser = CSVParser("data/kmeans_training_set.csv");
    KMeans kmeans = KMeans(parser.parseCSV());
    kmeans.initCentroids(CENTROID_COUNT);
}
