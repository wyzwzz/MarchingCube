//
// Created by wyz on 2021/5/27.
//

#ifndef MARCHINGCUBES_SURFACE_RENDER_HPP
#define MARCHINGCUBES_SURFACE_RENDER_HPP

#include<Render/shader_program.hpp>
#include<GLFW/glfw3.h>
#include<iostream>
#include<MarchingCubes/isosurface.hpp>

namespace mc {

    class Camera;

    class SurfaceRenderer {
    public:
        SurfaceRenderer(int w = 1200, int h = 900);

        ~SurfaceRenderer();

        void render(const IsoSurface<float> &isosurface);

    private:
        void initGL();

        void render();

        void createShader();

        void setupControl();

    private:
        std::unique_ptr<mc::Camera> camera;
        std::unique_ptr<mc::Shader> shader;
        size_t vertex_num;
        GLuint vao;
        GLuint vbo;

        GLFWwindow *window;
        int window_w, window_h;
        float min_board_x, min_board_y, min_board_z;
        float max_board_x, max_board_y, max_board_z;
    };


}
#endif //MARCHINGCUBES_SURFACE_RENDER_HPP
