#pragma once

#include <complex>
#include <vector>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/complex.hpp>

using complexe = std::complex<double>;
/**
 * Class representing a gesture, containing the complex Fourier descriptors
 */
struct Gesture {

    // Allow boost to serialize this class
    friend class boost::serialization::access;

    template<class Archive>
        void serialize(Archive & ar, const unsigned int version)
        {
            ar & f_descriptors;
            ar & name_class;
        }


    std::vector<complexe> f_descriptors;
    std::string name_class;

    Gesture()                             = default;
    Gesture(const Gesture& cp)            = default;
    Gesture& operator=(const Gesture& cp) = default;
    Gesture(Gesture&& cp)                 = default;
    Gesture& operator=(Gesture&& cp)      = default;

    Gesture(size_t n) {
        init_f_descriptors(n);
    }

    Gesture(std::string name, const std::vector<complexe> &descriptors) : 
        name_class(name), f_descriptors(descriptors) {}

    // Init at zeros the n (2*cmax + 1) Fourier descriptors
    void init_f_descriptors(size_t n) {
        for(size_t i = 0; i < n; i++)
            f_descriptors.push_back(complexe(0));
    }
};
