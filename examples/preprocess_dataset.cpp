#include <list>
#include <memory>
#include <stdlib.h>
#include "opencv2/opencv.hpp"

#include "definition.hpp"
#include "dataset_imgs.hpp"
#include "tools.hpp"

int main(int argc, char **argv)
{
    if (argc != 2)
    {
        std::cout << " Usage: " << argv[0]
                  << " outline_threshold (range: 0-80, default: 25)" << std::endl;
        return EXIT_FAILURE;
    }

    std::string self_path = get_self_dir();
    // dir folder containing the folders "left_hand", .. containing the images (sorted by type of gesture)
    std::string dataset_path = self_path + "/../../dataset";
    std::cout << "Looking for the dataset at: " << dataset_path << std::endl
              << "It should contain the following folders: " << std::endl;

    for (const std::string &gesture_name : gesture_classes)
    {
        std::cout << "raw_" << gesture_name << " containing the images, and "
                  << gesture_name << " empty" << std::endl;
    }

    int red_saturation_threshold = std::strtol(argv[1], NULL, 25);

    try
    {
        preProcessGesturesImgs(dataset_path, red_saturation_threshold);
    }
    catch (std::invalid_argument e)
    {
        std::cerr << std::endl
                  << "Error while getting the dataset: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }
    return 0;
}
