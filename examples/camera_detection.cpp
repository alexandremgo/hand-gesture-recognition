/**
 * Use to find the appropriate threshold value to get 
 * the best hand outline with the user's camera
 */
#include <memory>

#include "opencv2/opencv.hpp"

#include "fourier_descriptors.hpp"
#include "outline.hpp"

#define OUTLINE_THRESHOLD_MAX 80
#define CMAX 10

int red_saturation_threshold;

int main(int argc, char **argv)
{
    std::cout << "Use " << argv[0] << " to find the appropriate threshold value to get "
                                      "the best hand outline with your camera"
              << std::endl;

    if (argc != 2)
    {
        std::cout << "Usage: " << argv[0] << " camera_id (use ls -ltrh /dev/video*)" << std::endl;
        return 0;
    }

    int camera_id = std::strtol(argv[1], NULL, 0);

    cv::VideoCapture cap(camera_id);
    if (!cap.isOpened())
    {
        std::cerr << "Could not open camera " << camera_id << std::endl;
        return EXIT_FAILURE;
    }

    cv::Mat outline_img;
    cv::namedWindow("outline", 1);
    cv::namedWindow("raw_camera", 1);

    // Add a trackbar to configure the threshold used to compute the outline
    red_saturation_threshold = 25;
    cv::createTrackbar("Red saturation threshold", "outline", &red_saturation_threshold, OUTLINE_THRESHOLD_MAX);

    for (;;)
    {
        std::cout << "Exploiting a new frame" << std::endl;

        cv::Mat frame;
        cap >> frame; // get a new frame from camera

        int Y = frame.rows,
            X = frame.cols;

        std::vector<cv::Point> largest_outline;
        bool foundOutline = getOutline(largest_outline, frame, red_saturation_threshold);

        if (!foundOutline)
        {
            std::cout << "No outline was found" << std::endl;
            continue;
        }

        // Getting the Fourier descriptors
        auto descriptors = std::make_shared<std::vector<complexe>>();
        getFourierDescriptors(descriptors, largest_outline, CMAX);

        // Could not get the image back
        // throw std::bad_alloc because of bad memory handle
        //auto descriptors_img = std::make_shared<cv::Mat>(cv::Mat::zeros(y_size_const, x_size_const, CV_8U));
        // getFourierDescriptorsWithImage(descriptors_img, descriptors, largest_outline, cmax, x_size, y_size);

        /*
    	for(auto d : *descriptors) {
        	std::cout << d << " | ";
    	}
    	std::cout << std::endl;
	    */

        // Display the outline image for calibration
        outline_img = cv::Mat::zeros(Y, X, CV_8UC1);
        std::vector<std::vector<cv::Point>> outlines;
        outlines.push_back(largest_outline);
        // Draw only the largest outline
        cv::drawContours(outline_img, outlines, 0, 255);

        cv::imshow("raw_camera", frame);
        cv::imshow("outline", outline_img);

        if (cv::waitKey(100) == 27)
        {
            std::cout << std::endl
                      << "Last outline threshold: " << red_saturation_threshold << std::endl;
            break;
        }
    }

    return 0;
}
