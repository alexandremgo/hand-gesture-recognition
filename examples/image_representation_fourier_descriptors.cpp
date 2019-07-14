#include <memory>

#include "fourier_descriptors.hpp"
#include "outline.hpp"

#define CMAX 10
#define OUTLINE_THRESHOLD_MAX 80

cv::Mat image;
int x_size;
int y_size;
int red_saturation_threshold;

int computeFourierDescriptors()
{
    std::vector<cv::Point> largest_outline;

    bool foundOutline = getOutline(largest_outline, image, red_saturation_threshold, true);

    if (!foundOutline)
    {
        std::cout << "No outline was found" << std::endl;
        return EXIT_FAILURE;
    }

    // Getting the Fourier descriptors
    auto descriptors = std::make_shared<std::vector<complexe>>();
    auto descriptors_img = std::make_shared<cv::Mat>(cv::Mat::zeros(y_size, x_size, CV_8U));
    getFourierDescriptorsWithImage(descriptors_img, descriptors, largest_outline, CMAX, x_size, y_size);

    return 0;
}

void onThresholdTrackbar(int, void *)
{
    std::cout << std::endl
              << "Computing with a red saturation threshold of "
              << red_saturation_threshold << std::endl;

    computeFourierDescriptors();
}

int main(int argc, char **argv)
{
    if (argc != 2)
    {
        std::cout << " Usage: " << argv[0] << " path_to_image" << std::endl;
        return EXIT_FAILURE;
    }

    image = cv::imread(argv[1], cv::IMREAD_COLOR); // Read the file

    if (!image.data)
    {
        std::cout << "Could not open or find the image " << argv[1] << std::endl;
        return EXIT_FAILURE;
    }

    y_size = image.rows;
    x_size = image.cols;

    // And display the image of the descriptors
    cv::namedWindow("Image", cv::WINDOW_NORMAL);
    cv::resizeWindow("Image", WINDOW_SIZE_WIDTH, WINDOW_SIZE_HEIGHT);
    cv::moveWindow("Image", WINDOW_SIZE_WIDTH * 2 + WINDOW_POS_OFFSET,
                   2 * WINDOW_SIZE_HEIGHT + WINDOW_POS_OFFSET);
    cv::imshow("Image", image);

    // Add a trackbar to configure the threshold used to compute the outline
    red_saturation_threshold = 25;
    cv::createTrackbar("Red saturation threshold", "Image",
                       &red_saturation_threshold, OUTLINE_THRESHOLD_MAX, onThresholdTrackbar);

    computeFourierDescriptors();

    while (true)
    {
        if (cv::waitKey(30) == 27)
            return 0;
    }
}
