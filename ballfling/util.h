#pragma once

// utility code

#include <algorithm>
#include <random>
#include <string>
#include <sstream>
#include <math.h>

namespace util {
    // inclusive
    inline int rnd(int min, int max) {
        if (min > max)
            return max;
        std::random_device r;
        std::default_random_engine e(r());
        std::uniform_int_distribution<int> uniform_dist(min, max);
        return uniform_dist(e);
    }

    // inclusive
    inline float rnd(float min, float max) {
        if (min > max)
            return max;
        std::random_device r;
        std::mt19937 e(r());
        std::uniform_real_distribution<float> uniform_dist(min, max);
        return uniform_dist(e);
    }

    template <typename T>
    inline const T& choose(const T &a, const T &b) {
        int choice = rnd(0, 1);
        return choice == 0 ? a : b;
    }

    template <typename T>
    inline const T& choose(const T &a, const T &b, const T &c) {
        int choice = rnd(0, 2);
        switch (choice) {
        case 0:
            return a;
            break;
        case 1:
            return b;
            break;
        case 2:
            return c;
            break;
        default: // to stop warnings
            return a;
        }
    }

    // inclusive
    template <typename T>
    inline bool in_range(T x, T min, T max) {
        return std::min(max, std::max(min, x)) == x;
    }

    template <typename T>
    inline std::string to_string(T val) {
        std::stringstream s;
        s << val;
        return s.str();
    }

    template <typename T>
    inline typename std::vector<T>::iterator remove_from_vector(const T &element, std::vector<T> &vec) {
        // find position of element
        auto elm = vec.begin();
        for (elm =vec.begin(); elm<vec.end(); ++elm) {
            if (*elm == element)
                break;
        }
        // remove
        return vec.erase(elm);
    }

    // inclusive
    template <typename T>
    inline T clamp(const T &n, const T &lower, const T &upper) {
        return std::max(lower, std::min(n, upper));
    }

    template <typename T>
    inline T distance(T x1, T y1, T x2, T y2) {
        return sqrtf(pow(x2 - x1, 2) + pow(y2 - y1, 2));
    }

    template <typename T>
    inline float len(T vec) {
        return sqrtf(pow(vec.x, 2) + pow(vec.y, 2));
    }

    template <typename T>
    inline float len3(T vec) {
        return sqrtf(pow(vec.x, 2) + pow(vec.y, 2) + pow(vec.z, 2));
    }

    template <typename T>
    inline T cross(T a, T b) {
        T cvec;
        cvec.x = a.y * b.z - a.z * b.y;
        cvec.y = a.z * b.x - a.x * b.z;
        cvec.z = a.x * b.y - a.y * b.x;
        return cvec;
    }

    template <typename T>
    inline float dot3(T a, T b) {
        return a.x * b.x + a.y * b.y + a.z * b.z;
    }

    template <typename T>
    inline float dot(T a, T b) {
        return a.x * b.x + a.y * b.y;
    }

    template <typename T>
    inline T normalize(T vec) {
        return vec / len(vec);
    }

    template <typename T>
    inline T normalize3(T vec) {
        return vec / len3(vec);
    }

    template <typename T>
    inline float len_squared(T vec) {
        return pow(vec.x, 2) + pow(vec.y, 2);
    }

    template <typename T>
    inline int round(T x) {
        return (int)(x + 0.5);
    }

    template <typename T>
    inline T reduce(const T &value, float percentage) {
        percentage = 1.f - percentage;
        return sign(value) * fabs(value * percentage);
    }

    template <typename T>
    inline T sign(const T &value) {
        return (value > static_cast<T>(0.0)) ? static_cast<T>(1.0) : static_cast<T>(-1.0);
    }

    template <typename T>
    inline std::string vec2_to_str(const T &vec) {
        std::stringstream s;
        s << "(" << vec.x << ", " << vec.y << ")";
        return s.str();
    }
}