//
// Created by karan on 9/2/2025.
//

#ifndef CPU_CPP_RAYTRACING_MATERIALS_H
#define CPU_CPP_RAYTRACING_MATERIALS_H
#include "common.h"
#include "hitable.h"
#include "material.h"
#include "ray.h"
#include "vec3.h"

class ray;
struct hit_record;

vec3 reflect(const vec3 &v, const vec3 &n) {
    return v - 2 * dot(v, n) * n;
}

class lambertian : public material {
public:
    vec3 albedo;
    lambertian(const vec3 &a) : albedo(a) {}
    virtual bool scatter(const ray &r_in, const hit_record &rec, vec3 &attenuation, ray &scattered) const {
        vec3 target = rec.p + rec.normal + random_in_unit_sphere();
        scattered = ray(rec.p, target - rec.p);
        attenuation = albedo;
        return true;
    }
};

class metal : public material {
public:
    vec3 albedo;
    float fuzz;
    metal(const vec3& a, float f) : albedo(a) {
        if (f < 1) fuzz = f; else fuzz = 1;
    }

    virtual bool scatter(const ray& r_in, const hit_record& rec, vec3& attenuation, ray& scattered) const {
        vec3 reflected = reflect(unit_vector(r_in.direction()),rec.normal);
        scattered = ray(rec.p, reflected+fuzz*random_in_unit_sphere());
        attenuation = albedo;
        return dot(scattered.direction(), rec.normal) > 0;
    }
};

bool refract(const vec3 & v,const vec3 & outward_normal, float ni_over_nt, vec3 & refracted) {
    vec3 uv = unit_vector(v);
    float dt = dot(uv, outward_normal);
    float discriminant = 1.0 - ni_over_nt * ni_over_nt * (1 - dt * dt);
    if (discriminant > 0) {
        refracted = ni_over_nt * (uv - outward_normal * dt) - outward_normal * sqrt(discriminant);
        return true;
    }
    return false;
}

float schlick(float cosine, float ref_idx) {
    float r0 = (1 - ref_idx) / (1 + ref_idx);
    r0 = r0 * r0;
    return r0 + (1 - r0) * pow((1 - cosine), 5);
}

class dielectric : public material {
public:
    float refraction_index;
    dielectric(float ri) : refraction_index(ri) {}

    virtual bool scatter(const ray& r_in, const hit_record& rec, vec3& attenuation, ray& scattered) const {
        vec3 outward_normal;
        vec3 reflected = reflect(unit_vector(r_in.direction()), rec.normal);
        float ni_over_nt;
        attenuation = vec3(1.0, 1.0, 1.0);
        float reflect_prob;
        vec3 refracted;
        float cosine;
        if (dot(r_in.direction(), rec.normal) > 0) {
            outward_normal = -rec.normal;
            ni_over_nt = refraction_index;
            cosine = refraction_index * dot(r_in.direction(), rec.normal) / r_in.direction().length();
        } else {
            outward_normal = rec.normal;
            ni_over_nt = 1.0 / refraction_index;
            cosine = -dot(r_in.direction(), rec.normal) / r_in.direction().length();
        }
        if (refract(r_in.direction(), outward_normal, ni_over_nt, refracted)) {
            reflect_prob = schlick(cosine, refraction_index);
        }else {
            scattered = ray(rec.p, reflected);
            reflect_prob = 1.0;
        }
        if (dist(gen) < reflect_prob) {
            scattered = ray(rec.p, reflected);
        } else {
            scattered = ray(rec.p, refracted);
        }
        return true;
    }
};
#endif //CPU_CPP_RAYTRACING_MATERIALS_H