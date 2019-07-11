#pragma once 

#include <list>
#include <vector>
#include <fstream>
#include <complex>

#include "opencv2/opencv.hpp"
#include "gesture.hpp"
#include "definition.hpp"

using paire_dist = std::pair<std::string, double>;

class Knn
{
    public:
        // contains Fourier descriptors + label of all our training images
        std::list<Gesture> known_gestures;

        double distance (const std::vector<complexe> X, const std::vector<complexe> Y);

        std::string predict(const int k, const std::vector<complexe> &new_image);

        Knn() {}
        Knn(const std::list<Gesture> &k_g): known_gestures(k_g) {}
};
