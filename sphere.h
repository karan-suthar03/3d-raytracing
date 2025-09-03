//
// Created by karan on 9/2/2025.
//

#ifndef CPU_CPP_RAYTRACING_SPHERE_H
#define CPU_CPP_RAYTRACING_SPHERE_H
#include "hitable.h"

class sphere: public hitable {
public:
    sphere() = default;
    vec3 center;
    float radius;
    material *mat_ptr;
    sphere(const vec3 &c, float r,material *mat) : center(c), radius(r), mat_ptr(mat){}
    virtual bool hit(const ray &r, float t_min, float t_max, hit_record &rec) const;
};

bool sphere::hit(const ray &r, float t_min, float t_max, hit_record &rec) const {
    vec3 oc = r.origin() - center;
    float a = dot(r.direction(), r.direction());
    float b = dot(oc, r.direction());
    float c = dot(oc, oc) - radius * radius;
    float discriminant = b * b - a * c;
    if (discriminant > 0) {
        float temp = (-b - sqrt(discriminant)) / a;
        if (temp < t_max && temp > t_min) {
            rec.t = temp;
            rec.p = r.point_at_parameter(rec.t);
            rec.normal = (rec.p - center) / radius;
            rec.mat_ptr = mat_ptr;
            return true;
        }
        temp = (-b + sqrt(discriminant)) / a;
        if (temp < t_max && temp > t_min) {
            rec.t = temp;
            rec.p = r.point_at_parameter(rec.t);
            rec.normal = (rec.p - center) / radius;
            rec.mat_ptr = mat_ptr;
            return true;
        }
    }
    return false;
}


#endif //CPU_CPP_RAYTRACING_SPHERE_H