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
    vec3 u, v, w;

    camera(vec3 lookfrom, vec3 lookat, vec3 vup, float vfov, float aspect) {
        // vfov = vertical field of view in degrees
        float theta = vfov * M_PI / 180.0f;
        float half_height = tan(theta/2);
        float half_width = aspect * half_height;

        origin = lookfrom;
        w = unit_vector(lookfrom - lookat); // forward
        u = unit_vector(cross(vup, w));   // right
        v = cross(w, u); // up

        lower_left_corner = origin - half_width*u - half_height*v - w;
        horizontal = 2 * half_width * u;
        vertical   = 2 * half_height * v;
    }

    ray get_ray(float s, float t) {
        return ray(origin, lower_left_corner + s*horizontal + t*vertical - origin);
    }
};

#endif //CPU_CPP_RAYTRACING_CAMERA_H
