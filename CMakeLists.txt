cmake_minimum_required(VERSION 3.16)
project(cpu_cpp_raytracing)

set(CMAKE_CXX_STANDARD 20)

# Source files
add_executable(cpu_cpp_raytracing
        main.cpp
        vec3.h
        ray.h
        hitable.h
        sphere.h
        hitable_list.h
        camera.h
        material.h
        common.h
        materials.h
        cube.h
        perlin_noise.h
)

# Path to raylib
set(RAYLIB_DIR "C:/raylib")   # adjust this if needed
set(RAYLIB_INCLUDE_DIR "${RAYLIB_DIR}/include")
set(RAYLIB_LIB_DIR "${RAYLIB_DIR}/lib")

target_include_directories(cpu_cpp_raytracing PRIVATE ${RAYLIB_INCLUDE_DIR})
target_link_directories(cpu_cpp_raytracing PRIVATE ${RAYLIB_LIB_DIR})

# Link libraries
target_link_libraries(cpu_cpp_raytracing PRIVATE raylib opengl32 gdi32 winmm)

# copy raylib.dll next to the executable after build
add_custom_command(TARGET cpu_cpp_raytracing POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${RAYLIB_LIB_DIR}/raylib.dll"
        $<TARGET_FILE_DIR:cpu_cpp_raytracing>
)
