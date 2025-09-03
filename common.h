//
// Created by karan on 9/2/2025.
//

#ifndef CPU_CPP_RAYTRACING_COMMON_H
#define CPU_CPP_RAYTRACING_COMMON_H
#include "vec3.h"
vec3 random_in_unit_sphere() {
    vec3 p;
    do {
        p = 2.0 * vec3(dist(gen),dist(gen),dist(gen)) - vec3(1, 1, 1);
    } while (p.squared_length() >= 1.0);
    return p;
}
#endif //CPU_CPP_RAYTRACING_COMMON_H