#pragma once

#include <list>
#include <map>
#include <memory>
#include "opencv2/opencv.hpp"

#include "definition.hpp"
#include "gesture.hpp"
#include "outline.hpp"
#include "fourier_descriptors.hpp"

using gestures_imgs_map = std::map<std::string, std::list<cv::Mat>>;

/**
 * Get the images with a certain extension, and put them in a list of Mat
 * cvReadFlag: flag for cv::imread. cv::IMREAD_COLOR or cv::IMREAD_GRAYSCALE
 */
void getImgs(std::list<cv::Mat> &images, const std::string path, 
        const std::string extension, int cvReadFlag = cv::IMREAD_GRAYSCALE);

// get the images as a Mat for each kind of gesture
void getGesturesImgs(gestures_imgs_map &gestures_imgs, 
        std::string dataset_path, int cvReadFlag = cv::IMREAD_GRAYSCALE);

// Preprocess the raw images to only save the contour
void preProcessGesturesImgs(std::string dataset_path, int seuil);

/** 
 * Get the descriptors for all the images for each gesture
 * The gestures_imgss are grayscale that are already contour
 */
void getKnownGestures(std::list<Gesture> &known_gestures, const gestures_imgs_map &gestures_imgs, int cmax);
