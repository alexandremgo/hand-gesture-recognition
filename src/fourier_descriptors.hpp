#pragma once
#include "opencv2/opencv.hpp"
#include <complex>
#include <memory>

using complexe = std::complex<double>;

/** 
 * Get back the image to look
 */
void getBackImage(std::shared_ptr<cv::Mat> &descriptors_img, const std::vector<complexe> &descriptors, complexe mean, int nb_descriptors, int cmax, bool reversed, int x_size, int y_size, double max_descriptor = 1.0);

/** 
 * Calculate the Fourier descriptors for a certain contour
 */
void getFourierDescriptors(std::shared_ptr<std::vector<complexe>> &f_descriptors,
                           const std::vector<cv::Point> &contour, int cmax, bool display_img = false,
                           int x_size = 0, int y_size = 0);

/**
 * Get Fourier descriptors and the associated image
 */
void getFourierDescriptorsWithImage(std::shared_ptr<cv::Mat> &descriptors_img,
                                    std::shared_ptr<std::vector<complexe>> &f_descriptors,
                                    const std::vector<cv::Point> &contour, int cmax, int x_size, int y_size);
