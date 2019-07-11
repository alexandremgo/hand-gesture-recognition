/*
 ** K-nearest neighbors
 */
#include "knn.hpp"
using namespace cv;

// for an input vector X, the distance is computed considering the concat vector (X_re,X_im)
double Knn::distance (const std::vector<complexe> X, const std::vector<complexe> Y)
{
    double distance = 0;
    double diff_real, diff_imag;
    
    for (size_t i = 0; i < X.size(); ++i)
    {
        diff_real = X[i].real() - Y[i].real();
        diff_imag = X[i].imag()-Y[i].imag();
        distance += diff_real*diff_real + diff_imag*diff_imag;
    } // here the distance is computed without splitting the vectors..

    return (std::sqrt(distance));
}

bool compareDist(const paire_dist &A, const paire_dist &B) {
    return A.second < B.second;
}

std::string Knn::predict(const int k, const std::vector<complexe> &new_image)
{
    std::string class_image = "negatif";
    std::vector<paire_dist> ngbs;

    if(known_gestures.size() == 0)
        return class_image;

    // Calculate the distance for each already known gestures
    for (const auto &k_g : known_gestures)
    {
        ngbs.push_back(
                paire_dist(
                    k_g.name_class,
                    distance(k_g.f_descriptors, new_image))
                );

        // std::cout << "Distance with " << k_g.name_class << " : " << distance(k_g.f_descriptors, new_image);
    }

    // Trie pour avoir les k plus proches
    // n.log(n)
    std::sort(ngbs.begin(), ngbs.end(), compareDist);

    // To calculate the number of close neighbors and their associated class
    std::map<std::string, int> scores;

    for (size_t i = 0; i < k; i++) {
        std::string curClass = ngbs[i].first;
        if(scores.count(curClass) == 0)
            scores[curClass] = 1;
        else
            scores[curClass]++;
    }

    // To get the best class
    int best_score = 0;
    for(std::map<std::string, int>::iterator iter = scores.begin(); iter != scores.end(); ++iter)
    {
        if(iter->second > best_score) {
            best_score = iter->second;
            class_image = iter->first;
        }
    }

    // Check that the selected class not negative [criteria = score < 80% AND 3 nearest neighbours have a different class]
    if (best_score <= 0.8*k && (ngbs[0].first != ngbs[1].first || ngbs[1].first != ngbs[2].first))
    {
        class_image = "negatif";
    }
        
    std::cout << std::endl << "Prediction of weight " << best_score << " over k = " << k << std::endl;
    return class_image;
}

