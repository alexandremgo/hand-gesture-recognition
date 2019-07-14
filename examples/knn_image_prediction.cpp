#include <list>
#include <memory>
#include "opencv2/opencv.hpp"

#include "tools.hpp"
#include "knn.hpp"
#include "outline.hpp"
#include "fourier_descriptors.hpp"
#include "definition.hpp"
#include "dataset_imgs.hpp"

int main(int argc, char **argv)
{
    if (argc != 2)
    {
        std::cout << " Usage: " << argv[0] << "path_to_image_to_predict" << std::endl;
        return -1;
    }

    int cmax = 10;
    int k = 7;

    // Get the image we want to predict
    std::string image_path = argv[1];
    if (!file_exists(image_path))
    {
        std::cerr << std::endl
                  << "Image " << image_path << " does not exist" << std::endl;
        return EXIT_FAILURE;
    }
    cv::Mat image_to_predict = cv::imread(image_path, cv::IMREAD_COLOR);

    std::string self_path = get_self_dir();
    // dir folder containing the folders "left_hand", .. containing the images (sorted by type of geste)
    std::string dataset_path = self_path + "/../../dataset"; //argv[1];
    std::cout << "Looking for the dataset at: " << dataset_path << std::endl;

    // Get the grayscale contour images for each kind of gestures
    gestures_imgs_map gestures_imgs;

    try
    {
        getGesturesImgs(gestures_imgs, dataset_path, cv::IMREAD_GRAYSCALE);
    }
    catch (std::invalid_argument e)
    {
        std::cerr << std::endl
                  << "An error occured: " << e.what() << std::endl
                  << "Please be sure that the dataset is located at: " << dataset_path << std::endl;
        return EXIT_FAILURE;
    }

    std::cout << std::endl
              << "Getting the Fourier descriptors for each gesture image"
              << std::endl;

    // Get the descriptors for all the images for each gesture
    std::list<Gesture> known_gestures;
    getKnownGestures(known_gestures, gestures_imgs, cmax);

    std::cout << std::endl
              << "Nb of known gestures: " << known_gestures.size() << std::endl;
    Knn knn(known_gestures);

    bool foundContour;

    std::vector<cv::Point> contour_to_predict;
    try
    {
        foundContour = getOutline(contour_to_predict, image_to_predict);
    }
    catch (cv::Exception &e)
    {
        std::cerr << "While getting contour: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    if (!foundContour)
    {
        std::cout << "No contour was found for the image to predict" << std::endl;
        return 0;
    }

    auto descriptors_to_predict = std::make_shared<std::vector<complexe>>();
    try
    {
        getFourierDescriptors(descriptors_to_predict, contour_to_predict, cmax);
    }
    catch (cv::Exception &e)
    {
        std::cerr << "While getting descriptors: " << e.what() << std::endl;
        return 1;
    }

    std::cout << std::endl
              << "Descriptors to predict : ";
    for (auto d : *descriptors_to_predict)
    {
        std::cout << d << " | ";
    }
    std::cout << std::endl;

    std::string name = knn.predict(k, *descriptors_to_predict);
    std::cout << "Prediction: " << name << std::endl;
    return 0;
}
