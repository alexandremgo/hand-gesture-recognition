#include "fourier_descriptors.hpp"

DescriptorsComputedParameters getFourierDescriptors(std::shared_ptr<std::vector<complexe>> &f_descriptors,
                                                    const std::vector<cv::Point> &outline, int cmax, bool display_img,
                                                    int x_size, int y_size)
{

    std::vector<complexe> complex_outline;

    bool reversed = false;
    size_t outline_size = outline.size();

    // Computes the mean value
    complexe mean(0, 0);
    for (auto const &p : outline)
    {
        complex_outline.push_back(complexe(p.x, p.y));
        mean.real(mean.real() + p.x);
        mean.imag(mean.imag() + p.y);
    }
    mean.real(mean.real() / outline_size);
    mean.imag(mean.imag() / outline_size);

    // Centers on 0
    for (auto &p : complex_outline)
    {
        p.real(p.real() - mean.real());
        p.imag(p.imag() - mean.imag());
    }

    std::vector<complexe> fft;
    cv::dft(complex_outline, fft, cv::DFT_COMPLEX_OUTPUT);

    size_t nb_descriptors = fft.size();

    if (display_img)
    {
        std::cout << "Outline size: " << outline_size << " | Nb of descriptors: " << nb_descriptors
                  << " | cmax: " << cmax << std::endl;
    }

    // Normalize by the nb of descriptors
    for (auto &coef : fft)
    {
        coef /= (nb_descriptors);
    }

    int cmin = -cmax;

    // Take the last cmax descriptors
    for (size_t i = 0; i < cmax; i++)
    {
        f_descriptors->push_back(fft[nb_descriptors + cmin + i]);
    }

    f_descriptors->push_back(complexe(0));

    // Take the first cmax descriptors
    // At i = 0, the value given from dft is unsignificant
    for (size_t i = 1; i <= cmax; i++)
    {
        f_descriptors->push_back(fft[i]);
    }

    if (display_img)
    {
        auto img_most_significant = std::make_shared<cv::Mat>(cv::Mat::zeros(y_size, x_size, CV_8U));
        getBackImage(img_most_significant, *f_descriptors, mean, nb_descriptors, cmax, reversed, x_size, y_size);
        
        std::string window_name_0 = "Most significant coefficients";
        cv::namedWindow(window_name_0, cv::WINDOW_NORMAL);
        cv::resizeWindow(window_name_0, WINDOW_SIZE_WIDTH, WINDOW_SIZE_HEIGHT);
        cv::moveWindow(window_name_0, 0, WINDOW_SIZE_HEIGHT + WINDOW_POS_OFFSET);
        cv::imshow(window_name_0, *img_most_significant);

        std::cout << nb_descriptors / 2 << ": " << fft[nb_descriptors / 2] << std::endl;
        std::cout << nb_descriptors / 2 - 1 << ": " << fft[nb_descriptors / 2 - 1] << std::endl;
        std::cout << nb_descriptors / 2 + 1 << ": " << fft[nb_descriptors / 2 + 1] << std::endl;
        std::cout << nb_descriptors + cmin << ": " << fft[nb_descriptors + cmin] << std::endl;
        std::cout << nb_descriptors - 1 << ": " << fft[nb_descriptors - 1] << std::endl;
        std::cout << 0 << ": " << fft.at(0) << std::endl;
        std::cout << 1 << ": " << fft[1] << std::endl;
        std::cout << cmax << ": " << fft[cmax] << std::endl;
        std::cout << cmax + 1 << ": " << fft[cmax + 1] << std::endl;
    }

    // Invariance from the path taken by the outline
    // k = -1 and k = 1 <=> [cmax-1] and [cmax+1]
    if (abs((*f_descriptors)[cmax - 1]) > abs((*f_descriptors)[cmax + 1]))
    {
        if (display_img)
            std::cout << "Need to reverse" << std::endl;

        std::reverse(std::begin(*f_descriptors), std::end(*f_descriptors));
        reversed = true;
    }

    // The max abs value is at k = 1 <=> [cmax + 1]
    double max_value = abs((*f_descriptors)[cmax + 1]);
    //double max_value = abs(*f_descriptors[2*cmax-1]);
    for (auto &coef : *f_descriptors)
    {
        coef /= (max_value);
    }

    // Phase correction to normalize wrt the rotation
    double cor_phase = std::arg((*f_descriptors)[cmax - 1] * (*f_descriptors)[cmax + 1]) / 2;
    if (display_img)
        std::cout << "Correction phase: " << cor_phase << std::endl;

    // Phase correction to normalize wrt to the origin
    // -> a shift from the origin = a pure wait
    double cor_theta = std::arg((*f_descriptors)[cmax + 1]);

    // k from cmin to cmax
    int k = cmin;
    const std::complex<double> i(0, 1);
    for (auto &coef : *f_descriptors)
    {
        coef *= std::exp(-i * cor_phase - i * (cor_theta * k));
        //    std::cout << k << ": " << coef << " et abs: " << abs(coef) << std::endl;
        k++;
    }

    if (display_img)
    {
        auto img_final_descriptors = std::make_shared<cv::Mat>(cv::Mat::zeros(y_size, x_size, CV_8U));
        getBackImage(img_final_descriptors, *f_descriptors, mean, nb_descriptors, cmax, reversed, x_size, y_size, max_value);
        
        std::string window_name_1 = "Final descriptors";
        cv::namedWindow(window_name_1, cv::WINDOW_NORMAL);
        cv::resizeWindow(window_name_1, WINDOW_SIZE_WIDTH, WINDOW_SIZE_HEIGHT);
        cv::moveWindow(window_name_1, WINDOW_SIZE_WIDTH + WINDOW_POS_OFFSET, WINDOW_SIZE_HEIGHT + WINDOW_POS_OFFSET);
        cv::imshow(window_name_1, *img_final_descriptors);
    }

    DescriptorsComputedParameters computed_parameters = {
        mean, nb_descriptors, reversed, max_value};

    return computed_parameters;
}

void getFourierDescriptorsWithImage(std::shared_ptr<cv::Mat> &descriptors_img,
                                    std::shared_ptr<std::vector<complexe>> &f_descriptors,
                                    const std::vector<cv::Point> &outline, int cmax, int x_size, int y_size)
{

    DescriptorsComputedParameters computed_parameters = getFourierDescriptors(
        f_descriptors, outline, cmax, true, x_size, y_size);

    getBackImage(descriptors_img, *f_descriptors, computed_parameters.mean,
                 computed_parameters.nb_descriptors, cmax, computed_parameters.reversed,
                 x_size, y_size, computed_parameters.max_descriptor);
}

void getBackImage(std::shared_ptr<cv::Mat> &descriptors_img,
                  const std::vector<complexe> &descriptors, complexe mean, size_t nb_descriptors,
                  int cmax, bool reversed, int x_size, int y_size, double max_descriptor)
{

    // Fill with zeros to get the initial size of the image
    std::vector<complexe> descriptors_with_zeros;
    // The first one was a 0
    descriptors_with_zeros.push_back(complexe(0));

    double coef_multi = ((double)nb_descriptors) * max_descriptor;
    // Take the first cmax descriptors that were put at the end
    for (size_t i = cmax + 1; i < 2 * cmax + 1; i++)
    {
        descriptors_with_zeros.push_back(coef_multi * descriptors[i]);
    }

    // + 1 because of the 0 at the beginning
    for (size_t i = cmax + 1; i < nb_descriptors - cmax; i++)
    {
        descriptors_with_zeros.push_back(complexe(0));
    }

    // Take the last cmax descriptors that were put at the beginning
    for (size_t i = 0; i < cmax; i++)
    {
        descriptors_with_zeros.push_back(coef_multi * descriptors[i]);
    }

    // Take into account reverse <- hmm actually no ?!
    //if(reversed)
    //    std::reverse(std::begin(descriptors_with_zeros), std::end(descriptors_with_zeros));

    // std::cout << "Nb of descriptors with zeros: " << descriptors_with_zeros.size() << std::endl;

    std::vector<complexe> c_outline;
    // DFT_SCALE: divide by the number of elements
    cv::dft(descriptors_with_zeros, c_outline, cv::DFT_INVERSE + cv::DFT_SCALE);

    std::vector<cv::Point> p_outline;
    for (auto const &c : c_outline)
    {
        // x = real part | y = imaginary part
        p_outline.push_back(cv::Point(c.real() + mean.real(), c.imag() + mean.imag()));
    }
    std::cout << std::endl;

    // Global outlines containing just one outline, for drawing
    std::vector<std::vector<cv::Point>> g_outlines;
    g_outlines.push_back(p_outline);

    //Mat img_outline = Mat::zeros(y_size, x_size, CV_32FC1);
    // Suppose it has already be defined
    //*descriptors_img = Mat::zeros(y_size, x_size, CV_8U);
    drawContours(*descriptors_img, g_outlines, 0, 255);
}