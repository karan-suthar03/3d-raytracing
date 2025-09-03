//
// Created by karan on 9/2/2025.
//

#ifndef CPU_CPP_RAYTRACING_RAY_H
#define CPU_CPP_RAYTRACING_RAY_H
#include "vec3.h"

class ray {
public:
    ray() = default;
    vec3 a;
    vec3 b;
    ray(const vec3 &a, const vec3 &b) : a(a), b(unit_vector(b)) {}
    vec3 origin() const {
        return a;
    }
    vec3 direction() const {
        return b;
    }
    vec3 point_at_parameter(float t) const {
        return a + t * b;
    }
};

#endif //CPU_CPP_RAYTRACING_RAY_H