//
// Created by karan on 9/2/2025.
//

#ifndef CPU_CPP_RAYTRACING_HITABLE_H
#define CPU_CPP_RAYTRACING_HITABLE_H
#include "ray.h"
#include "vec3.h"

class material;

struct hit_record {
    float t;
    vec3 p;
    vec3 normal;
    material *mat_ptr;
};

class hitable {
public:
    virtual bool hit(const ray &r, float t_min, float t_max, hit_record &rec) const = 0;
};

#endif //CPU_CPP_RAYTRACING_HITABLE_H