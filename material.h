//
// Created by karan on 9/2/2025.
//

#ifndef CPU_CPP_RAYTRACING_MATERIAL_H
#define CPU_CPP_RAYTRACING_MATERIAL_H

struct hit_record;
class vec3;
class ray;

class material {
public:
    virtual bool scatter(const ray& r_in, const hit_record& rec, vec3& attenuation, ray& scattered) const = 0;
};

#endif //CPU_CPP_RAYTRACING_MATERIAL_H