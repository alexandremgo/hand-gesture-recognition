#pragma once
#include <string>

const int nb_gestures = 5;

enum gesture_ids
{
    open_hand = 0,
    left_hand = 1,
    right_hand = 2,
    fist = 3,
    negative = 4
};

const std::string gesture_classes[nb_gestures] = {"open_hand", "left_hand", "right_hand", "fist", "negative"};
