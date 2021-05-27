//
// Created by wyz on 2021/5/27.
//

#ifndef MARCHINGCUBES_SURFACE_RENDER_HPP
#define MARCHINGCUBES_SURFACE_RENDER_HPP
#include<Render/shader_program.hpp>
#include<GLFW/glfw3.h>
#include<iostream>
#include<MarchingCubes/isosurface.hpp>

namespace mc{

    class SurfaceRenderer{
    public:
        SurfaceRenderer()=default;
        void render(const IsoSurface<float>& isosurface);

    private:
        void initGL();
        void render();
        void createShader();
    private:
        std::unique_ptr<mc::Shader> shader;
        size_t vertex_num;
        GLuint vao;
        GLuint vbo;

        GLFWwindow *window;
    };




}
#endif //MARCHINGCUBES_SURFACE_RENDER_HPP
