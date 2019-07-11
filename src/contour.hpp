#pragma once
#include "opencv2/opencv.hpp"
#include <complex>
#include <vector>

/**
 * Gets the largest body (object with more red color) outline from the image
 * 
 * param: display_imgs = display the original, detection and outline images
 * 
 * return: true if found at least on outline, false otherwise
 */
bool getOutline(std::vector<cv::Point> &largest_outline, const cv::Mat &image, 
        int red_saturation_threshold = 25, bool display_imgs = false);

/**
 * Gets the outline from a grayscale img where the outline was drawn in white
 */
bool getOutlineFromBinaryImg(std::vector<cv::Point> &largest_outline, const cv::Mat &binary_img);
