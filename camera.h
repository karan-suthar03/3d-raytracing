//
// Created by karan on 9/2/2025.
//


#include <random>
std::random_device rd;
std::mt19937 gen(rd()); // Mersenne Twister RNG
std::uniform_real_distribution<float> dist(0.0f, 1.0f);

#ifndef CPU_CPP_RAYTRACING_CAMERA_H
#define CPU_CPP_RAYTRACING_CAMERA_H
#include "ray.h"
#include "vec3.h"

class camera {
public:
    vec3 origin;
    vec3 lower_left_corner;
    vec3 horizontal;
    vec3 vertical;
    camera() {
        lower_left_corner = vec3(-2.0, -1.0, -1.0);
        horizontal = vec3(4.0, 0.0, 0.0);
        vertical = vec3(0.0, 2.0, 0.0);
        origin = vec3(0.0, 0.0, 0.0);
    }
    ray get_ray(float u, float v) {
        return ray(origin, lower_left_corner + u*horizontal + v*vertical);
    }
};

#endif //CPU_CPP_RAYTRACING_CAMERA_H