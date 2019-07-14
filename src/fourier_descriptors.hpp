#pragma once
#include "opencv2/opencv.hpp"
#include <complex>
#include <memory>

#define WINDOW_SIZE_WIDTH 400
#define WINDOW_SIZE_HEIGHT 400
#define WINDOW_POS_OFFSET 10

using complexe = std::complex<double>;

struct DescriptorsComputedParameters
{
    complexe mean;
    size_t nb_descriptors;
    bool reversed;
    double max_descriptor;
};

/** 
 * Compute the Fourier descriptors of an outline, with a possibility to display values and images
 * during the process
 * 
 * param: cmax = number of Fourier coefficients, from the Fourier serie, that will be kept (2*cmax)
 */
DescriptorsComputedParameters getFourierDescriptors(std::shared_ptr<std::vector<complexe>> &f_descriptors,
                                                    const std::vector<cv::Point> &outline, int cmax, bool display_img = false,
                                                    int x_size = 0, int y_size = 0);

/**
 * Compute the Fourier descriptors and the associated image representation of the 
 * Fourier descriptors from an outline
 * 
 * param: cmax = number of Fourier coefficients, from the Fourier serie, that will be kept (2*cmax)
 */
void getFourierDescriptorsWithImage(std::shared_ptr<cv::Mat> &descriptors_img,
                                    std::shared_ptr<std::vector<complexe>> &f_descriptors,
                                    const std::vector<cv::Point> &outline, int cmax, int x_size, int y_size);

/** 
 * Get back the image from the Fourier descriptors
 */
void getBackImage(std::shared_ptr<cv::Mat> &descriptors_img,
                  const std::vector<complexe> &descriptors, complexe mean, size_t nb_descriptors,
                  int cmax, bool reversed, int x_size, int y_size, double max_descriptor = 1.0);
