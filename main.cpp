#include <thread>
#include <mutex>
#include <atomic>
#include <cmath>
#include <condition_variable>
#include <vector>
#include <deque>

#include "camera.h"
#include "cube.h"
#include "hitable.h"
#include "hitable_list.h"
#include "materials.h"
#include "ray.h"
#include "raylib.h"
#include "sphere.h"

std::mutex pixel_mutex;
std::atomic<int> lines_done(0);
std::atomic<bool> rendering_done(false);

struct Task {
    int start_y;
    int end_y;
};

std::mutex task_mutex;
std::condition_variable task_cv;
std::deque<Task> tasks;
bool all_done = false;

vec3 color(const ray& r, hitable *world, int depth) {
    hit_record rec;
    if (world->hit(r, 0.001, INFINITY, rec)) {
        ray scattered;
        vec3 attenuation;
        if (depth < 50 && rec.mat_ptr->scatter(r, rec, attenuation, scattered)) {
            return attenuation * color(scattered, world, depth + 1);
        }
        return vec3(0, 0, 0);
    }
    vec3 unit_direction = unit_vector(r.direction());
    double t = 0.5 * (unit_direction.y() + 1.0);
    return (1.0 - t) * vec3(1.0, 1.0, 1.0) + t * vec3(0.5, 0.7, 1.0);
}

void render_scanlines(Color* pixels, std::vector<vec3>& accum, int nx, int ny, hitable* world, camera& cam, int current_sample, int start_y, int end_y) {
    for (int j = start_y; j < end_y; j++) {
        for (int i = 0; i < nx; i++) {
            float u = float(i + dist(gen)) / float(nx);
            float v = float(j + dist(gen)) / float(ny);
            ray r = cam.get_ray(u, v);
            vec3 c = color(r, world, 0);

            int idx = (ny - 1 - j) * nx + i;
            accum[idx] += c;  // accumulate sample

            vec3 avg = accum[idx] / float(current_sample);
            avg = vec3(sqrt(avg[0]), sqrt(avg[1]), sqrt(avg[2])) * 255.99f;

            pixels[idx] = {
                (unsigned char)std::clamp(avg[0], 0.0f, 255.0f),
                (unsigned char)std::clamp(avg[1], 0.0f, 255.0f),
                (unsigned char)std::clamp(avg[2], 0.0f, 255.0f),
                255
            };
        }
    }
}

void render_pass(Color* pixels, std::vector<vec3>& accum, int nx, int ny,
                 hitable* world, camera& cam, int current_sample) {
    int num_threads = std::thread::hardware_concurrency();
    if (num_threads == 0) num_threads = 4;

    std::vector<std::thread> pool;
    int rows_per_thread = ny / num_threads;

    for (int t = 0; t < num_threads; t++) {
        int start_y = t * rows_per_thread;
        int end_y = (t == num_threads - 1) ? ny : start_y + rows_per_thread;
        pool.emplace_back(render_scanlines, pixels, std::ref(accum), nx, ny,
                          world, std::ref(cam), current_sample, start_y, end_y);
    }

    for (auto& th : pool) th.join();
}

int main() {
    const int nx = 1440;
    const int ny = 720;
    // const int ns = 100;

    InitWindow(nx, ny, "Raytracing");
    SetTargetFPS(60);

    hitable* list[4];
    list[0] = new sphere(vec3(0, 0, -1), 0.25, new lambertian(vec3(0.8, 0.3, 0.3)));
    list[1] = new sphere(vec3(0, -100.5, -1), 100, new lambertian(vec3(0.8, 0.8, 0.0)));
    list[2] = new cube(vec3(-0.7,0,-1),0.5,new metal(vec3(0.8,0.6,0.2),0.01));
    list[3] = new cube(vec3(0.7,0,-1),0.5,new dielectric(1.5));
    hitable* world = new hitable_list(list, 4);

    Image img = GenImageColor(nx, ny, BLACK);
    Color* pixels = (Color*)img.data;
    camera cam;

    Texture2D texture = LoadTextureFromImage(img);

    std::vector<vec3> accum(nx * ny, vec3(0,0,0)); // buffer
    int sample_count = 0;

    while (!WindowShouldClose()) {
        sample_count++;

        // render one frame per frame
        render_pass(pixels, accum, nx, ny, world, cam, sample_count);

        UpdateTexture(texture, pixels);

        BeginDrawing();
        ClearBackground(BLACK);
        DrawTexture(texture, 0, 0, WHITE);
        DrawText(TextFormat("samples done: %d", sample_count), 10, 10, 20, Color(0,0,0,255));
        EndDrawing();
    }
    // task_cv.notify_all();

    // render_thread.join();
    UnloadTexture(texture);
    CloseWindow();
    return 0;
}
