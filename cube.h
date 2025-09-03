//
// Created by karan on 9/2/2025.
//

#ifndef CPU_CPP_RAYTRACING_CUBE_H
#define CPU_CPP_RAYTRACING_CUBE_H
#include "hitable.h"
#include <algorithm>

struct triangle {
    vec3 v0, v1, v2;
    vec3 normal;
};

// unit cube
const vec3 unit_vertices[8] = {
    vec3(-0.5f, -0.5f, -0.5f),
    vec3( 0.5f, -0.5f, -0.5f),
    vec3( 0.5f,  0.5f, -0.5f),
    vec3(-0.5f,  0.5f, -0.5f),
    vec3(-0.5f, -0.5f,  0.5f),
    vec3( 0.5f, -0.5f,  0.5f),
    vec3( 0.5f,  0.5f,  0.5f),
    vec3(-0.5f,  0.5f,  0.5f)
};

// unit triangles
const triangle unit_triangles[12] = {
    // Front face (z = -0.5)
    {vec3(-0.5f, -0.5f, -0.5f), vec3(0.5f, -0.5f, -0.5f), vec3(0.5f, 0.5f, -0.5f), vec3(0,0,-1)},
    {vec3(-0.5f, -0.5f, -0.5f), vec3(0.5f, 0.5f, -0.5f), vec3(-0.5f, 0.5f, -0.5f), vec3(0,0,-1)},
    // Back face (z = 0.5)
    {vec3(-0.5f, -0.5f, 0.5f), vec3(0.5f, -0.5f, 0.5f), vec3(0.5f, 0.5f, 0.5f), vec3(0,0,1)},
    {vec3(-0.5f, -0.5f, 0.5f), vec3(0.5f, 0.5f, 0.5f), vec3(-0.5f, 0.5f, 0.5f), vec3(0,0,1)},
    // Left face (x = -0.5)
    {vec3(-0.5f, -0.5f, -0.5f), vec3(-0.5f, 0.5f, -0.5f), vec3(-0.5f, 0.5f, 0.5f), vec3(-1,0,0)},
    {vec3(-0.5f, -0.5f, -0.5f), vec3(-0.5f, 0.5f, 0.5f), vec3(-0.5f, -0.5f, 0.5f), vec3(-1,0,0)},
    // Right face (x = 0.5)
    {vec3(0.5f, -0.5f, -0.5f), vec3(0.5f, 0.5f, -0.5f), vec3(0.5f, 0.5f, 0.5f), vec3(1,0,0)},
    {vec3(0.5f, -0.5f, -0.5f), vec3(0.5f, 0.5f, 0.5f), vec3(0.5f, -0.5f, 0.5f), vec3(1,0,0)},
    // Bottom face (y = -0.5)
    {vec3(-0.5f, -0.5f, -0.5f), vec3(0.5f, -0.5f, -0.5f), vec3(0.5f, -0.5f, 0.5f), vec3(0,-1,0)},
    {vec3(-0.5f, -0.5f, -0.5f), vec3(0.5f, -0.5f, 0.5f), vec3(-0.5f, -0.5f, 0.5f), vec3(0,-1,0)},
    // Top face (y = 0.5)
    {vec3(-0.5f, 0.5f, -0.5f), vec3(0.5f, 0.5f, -0.5f), vec3(0.5f, 0.5f, 0.5f), vec3(0,1,0)},
    {vec3(-0.5f, 0.5f, -0.5f), vec3(0.5f, 0.5f, 0.5f), vec3(-0.5f, 0.5f, 0.5f), vec3(0,1,0)}
};

class cube : public hitable {
public:
    material *mat_ptr;
    vec3 vertices[8];
    triangle triangles[12];

    cube(const vec3 &position, float size, material *mat) {
        mat_ptr = mat;
        for (int i = 0; i < 8; ++i) {
            vertices[i] = unit_vertices[i] * size + position;
        }
        for (int i = 0; i < 12; ++i) {
            triangles[i].v0 = unit_triangles[i].v0 * size + position;
            triangles[i].v1 = unit_triangles[i].v1 * size + position;
            triangles[i].v2 = unit_triangles[i].v2 * size + position;
            triangles[i].normal = unit_triangles[i].normal;
        }
    }
    virtual bool hit(const ray &r, float t_min, float t_max, hit_record &rec) const;
};

bool ray_triangle_intersect(const ray &r, const triangle &tri, float t_min, float t_max, float &t, vec3 &normal) {
    constexpr float epsilon = std::numeric_limits<float>::epsilon();

    vec3 edge1 = tri.v1 - tri.v0;
    vec3 edge2 = tri.v2 - tri.v0;

    vec3 pvec = cross(r.direction(), edge2);
    float det = dot(edge1, pvec);

    if (det > -epsilon && det < epsilon)
        return false; // Ray parallel to triangle

    float inv_det = 1.0f / det;
    vec3 tvec = r.origin() - tri.v0;
    float u = inv_det * dot(tvec, pvec);

    if (u < 0.0f || u > 1.0f)
        return false;

    vec3 qvec = cross(tvec, edge1);
    float v = inv_det * dot(r.direction(), qvec);

    if (v < 0.0f || u + v > 1.0f)
        return false;

    t = inv_det * dot(edge2, qvec);

    // vec3 p = r.point_at_parameter(t); // hit point



    if (t > t_min && t < t_max)
    {
        // vec3 perlin_vec = vec3(ValueNoise_2D(x*500,y*500),ValueNoise_2D(x*500,y*500),ValueNoise_2D(x*500,y*500));
        // perlin_vec.make_unit_vector();
        // normal = tri.normal+perlin_vec;
        normal = tri.normal;
        return true;
    }

    return false;
}

bool cube::hit(const ray &r, float t_min, float t_max, hit_record &rec) const {
    float closest_t = t_max;
    bool hit_anything = false;
    vec3 hit_normal;

    for (const auto &tri : this->triangles) {
        float t;
        vec3 normal;
        if (ray_triangle_intersect(r, tri, t_min, closest_t, t, normal)) {
            hit_anything = true;
            closest_t = t;
            hit_normal = normal;
        }
    }

    if (hit_anything) {
        rec.t = closest_t;
        rec.p = r.point_at_parameter(closest_t);
        rec.normal = hit_normal;
        rec.mat_ptr = mat_ptr;
        return true;
    }
    return false;
}


#endif //CPU_CPP_RAYTRACING_CUBE_H