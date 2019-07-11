#include "dataset_imgs.hpp"

void getImgs(std::list<cv::Mat> &images, const std::string path,
             const std::string extension, int cvReadFlag)
{
    std::string folderpath = path + "/*." + extension;
    std::vector<cv::String> filenames;

    try
    {
        cv::glob(folderpath, filenames);
    }
    catch (cv::Exception e)
    {
        throw std::invalid_argument(path + " was not found");
    }

    for (size_t i = 0; i < filenames.size(); i++)
    {
        images.push_back(cv::imread(filenames[i], cvReadFlag));
    }
}

void getGesturesImgs(gestures_imgs_map &gestures_imgs,
                     std::string dataset_path, int cvReadFlag)
{
    std::string extension = "jpg";

    // For each gestures, get the images
    for (auto g_name : gesture_classes)
    {
        std::list<cv::Mat> curList;

        try
        {
            getImgs(curList, dataset_path + "/" + g_name, extension, cvReadFlag);
        }
        catch (std::invalid_argument e)
        {
            throw;
        }

        gestures_imgs[g_name] = curList;

        std::cout << "Nb imgs for " << g_name << ": "
                  << curList.size() << std::endl;
    }
}

void preProcessGesturesImgs(std::string dataset_path, int red_saturation_threshold)
{
    std::string extension = "jpg";
    bool found_outline;

    // For each gestures, get the images
    for (auto g_name : gesture_classes)
    {
        // Loop over each gesture folder containing raw images
        std::string raw_folderpath = dataset_path + "/" + "raw_" + g_name + "/*." + extension;
        std::string folderpath = dataset_path + "/" + g_name;
        std::vector<cv::String> filenames;

        std::cout << std::endl
                  << "####################" << std::endl
                  << "Working on " << folderpath << std::endl
                  << "####################" << std::endl;

        try
        {
            cv::glob(raw_folderpath, filenames);
        }
        catch (cv::Exception e)
        {
            throw std::invalid_argument(raw_folderpath + " were not found");
        }

        int nb_notFound = 0;
        for (auto f_name : filenames)
        {
            // Get the image
            cv::Mat curImg = cv::imread(f_name, cv::IMREAD_COLOR);
            // Compute its largest outline
            std::vector<cv::Point> largest_outline;

            // Find the name with the extension
            size_t start_name_pos = f_name.find_last_of("/");
            cv::String img_name = cv::String(f_name.begin() + start_name_pos, f_name.end());
            // std::cout << "Image name: " << img_name << std::endl;

            try
            {
                found_outline = getOutline(largest_outline, curImg, red_saturation_threshold);
            }
            catch (cv::Exception &e)
            {
                std::cerr << "For " << img_name << ": error while getting contour: " << e.what() << std::endl;
                continue;
            }

            if (!found_outline)
            {
                std::cout << "For " << img_name << ": no contour was found" << std::endl;
                nb_notFound++;
                continue;
            }

            cv::Mat contour_img = cv::Mat::zeros(curImg.rows, curImg.cols, CV_8UC1);
            std::vector<std::vector<cv::Point>> contours;
            contours.push_back(largest_outline);
            // Draw only the largest contour
            cv::drawContours(contour_img, contours, 0, 255);

            // And save the image
            cv::imwrite(folderpath + img_name, contour_img);
        }

        std::cout << std::endl
                  << "For " << g_name << " added "
                  << filenames.size() - nb_notFound << " images"
                  << std::endl
                  << std::endl;
    }
}

void getKnownGestures(std::list<Gesture> &known_gestures, const gestures_imgs_map &gestures_imgs, int cmax)
{
    bool found_outline;

    // get the known gestures
    for (std::map<std::string, std::list<cv::Mat>>::const_iterator iter = gestures_imgs.begin(); iter != gestures_imgs.end(); ++iter)
    {
        for (const auto img : iter->second)
        {

            // std::cout << std::endl << "Working on: " << iter->first << std::endl;
            std::vector<cv::Point> contour;
            try
            {
                // The images are grayscale that are already contour
                found_outline = getOutlineFromBinaryImg(contour, img);
            }
            catch (cv::Exception &e)
            {
                std::cerr << "While getting contour: " << e.what() << std::endl;
                return;
            }

            if (!found_outline)
            {
                std::cout << "No contour was found for a " << iter->first << std::endl;
                break;
            }

            // std::cout << "Contour composed of: " << contour.size() << " points" << std::endl;

            auto descriptors = std::make_shared<std::vector<complexe>>();
            try
            {
                getFourierDescriptors(descriptors, contour, cmax);
            }
            catch (cv::Exception &e)
            {
                std::cerr << "While getting descriptors: " << e.what() << std::endl;
                return;
            }

            known_gestures.push_back(Gesture(iter->first, *descriptors));
        }
    }
}
