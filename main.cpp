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

void render_scanlines(Color* pixels, int nx, int ny, int ns, hitable* world, camera& cam, int start_y, int end_y) {
    for (int j = start_y; j < end_y; j++) {
        for (int i = 0; i < nx; i++) {
            vec3 c(0, 0, 0);
            for (int s = 0; s < ns; s++) {
                float u = float(i + dist(gen)) / float(nx);
                float v = float(j + dist(gen)) / float(ny);
                ray r = cam.get_ray(u, v);
                c += color(r, world, 0);
            }
            c /= float(ns);
            c = vec3(sqrt(c[0]), sqrt(c[1]), sqrt(c[2]));
            c *= 255.99;

            {
                std::lock_guard<std::mutex> lock(pixel_mutex);
                pixels[(ny - 1 - j) * nx + i] = {
                    (unsigned char)c[0],
                    (unsigned char)c[1],
                    (unsigned char)c[2],
                    255
                };
            }
        }
        lines_done++;
    }
}

void worker(Color* pixels, int nx, int ny, int ns, hitable* world, camera& cam) {
    while (true) {
        Task task;
        {
            std::unique_lock lock(task_mutex);
            if (tasks.empty()) {
                if (all_done) break;
                task_cv.wait(lock, [] { return !tasks.empty() || all_done; });
                if (all_done && tasks.empty()) break;
            }
            task = tasks.front();
            tasks.pop_front();
        }

        int count = task.end_y - task.start_y;
        if (count > 1) {
            // split into two smaller tasks
            int mid = (task.start_y + task.end_y) / 2;
            Task half1{task.start_y, mid};
            Task half2{mid, task.end_y};

            {
                std::lock_guard lock(task_mutex);
                tasks.push_back(half2);   // push one half back
                tasks.push_back(half1);   // push the other half back
            }
            task_cv.notify_all();
            continue;  // pick next task from queue
        }
        // single line -> render
        render_scanlines(pixels, nx, ny, ns, world, cam, task.start_y, task.end_y);
        lines_done++;
    }
}

int main() {
    const int nx = 1440;
    const int ny = 720;
    const int ns = 100;

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

    {
        std::lock_guard<std::mutex> lock(task_mutex);
        tasks.push_back({0, ny});
    }

    // spawn worker pool
    int num_threads = std::thread::hardware_concurrency();
    if (num_threads == 0) num_threads = 4;
    std::vector<std::thread> pool;
    for (int i = 0; i < num_threads; i++) {
        pool.emplace_back(worker, pixels, nx, ny, ns, world, std::ref(cam));
    }
    // std::cout<< "perlin::" << ValueNoise_2D(20,20);

    while (!WindowShouldClose()) {
        {
            std::lock_guard<std::mutex> lock(pixel_mutex);
            UpdateTexture(texture, pixels);
        }

        BeginDrawing();
        ClearBackground(BLACK);
        DrawTexture(texture, 0, 0, WHITE);
        DrawText(TextFormat("Lines done: %d/%d", lines_done.load(), ny), 10, 10, 20, RAYWHITE);
        EndDrawing();

        if (rendering_done) {
            // keep showing final result until window closed
        }
    }

    {
        std::lock_guard<std::mutex> lock(task_mutex);
        all_done = true;
    }
    task_cv.notify_all();

    // render_thread.join();
    UnloadTexture(texture);
    CloseWindow();
    return 0;
}
