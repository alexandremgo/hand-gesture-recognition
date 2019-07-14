#include <list>
#include <memory>
#include "opencv2/opencv.hpp"

#include "tools.hpp"
#include "knn.hpp"
#include "outline.hpp"
#include "fourier_descriptors.hpp"
#include "definition.hpp"
#include "dataset_imgs.hpp"

#define DESCRIPTORS_CMAX 10
#define KNN_K 7
#define OUTLINE_THRESHOLD_MAX 80

int red_saturation_threshold;

int initKnn(Knn &knn)
{
    std::cout << std::endl
              << "Initializing the KNN model" << std::endl;

    std::string self_path = get_self_dir();
    // dir folder containing the folders "left_hand", .. containing the images (sorted by type of gesture)
    std::string dataset_path = self_path + "/../../dataset";
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
    getKnownGestures(known_gestures, gestures_imgs, DESCRIPTORS_CMAX);

    std::cout << std::endl
              << "Nb of known gestures: " << known_gestures.size() << std::endl;

    knn.known_gestures = known_gestures;

    return 0;
}

int main(int argc, char **argv)
{
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

    // Init the KNN model
    Knn knn;
    if (initKnn(knn) == EXIT_FAILURE)
        EXIT_FAILURE;

    // Working on the input frames
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
        cv::imshow("raw_camera", frame);

        int Y = frame.rows,
            X = frame.cols;

        bool foundOutline;

        std::vector<cv::Point> largest_outline;
        try
        {
            foundOutline = getOutline(largest_outline, frame, red_saturation_threshold);
        }
        catch (cv::Exception &e)
        {
            std::cerr << "While getting contour: " << e.what() << std::endl;
            continue;
        }

        if (!foundOutline)
        {
            std::cout << "No contour was found for the image to predict" << std::endl;
            continue;
        }

        // Display the outline image for calibration
        outline_img = cv::Mat::zeros(Y, X, CV_8UC1);
        std::vector<std::vector<cv::Point>> outlines;
        outlines.push_back(largest_outline);
        // Draw only the largest outline
        cv::drawContours(outline_img, outlines, 0, 255);
        cv::imshow("outline", outline_img);

        auto descriptors_to_predict = std::make_shared<std::vector<complexe>>();
        try
        {
            getFourierDescriptors(descriptors_to_predict, largest_outline, DESCRIPTORS_CMAX);
        }
        catch (cv::Exception &e)
        {
            std::cerr << "While getting descriptors: " << e.what() << std::endl;
            continue;
        }

        std::cout << std::endl
                  << "Descriptors to predict : ";
        for (auto d : *descriptors_to_predict)
        {
            std::cout << d << " | ";
        }
        std::cout << std::endl;

        std::string name = knn.predict(KNN_K, *descriptors_to_predict);
        std::cout << "Prediction: " << name << std::endl
                  << std::endl;

        //cv::imshow("outline", outline_img);

        if (cv::waitKey(200) == 27)
        {
            std::cout << std::endl
                      << "Bisous" << std::endl;
            break;
        }
    }

    return 0;
}
