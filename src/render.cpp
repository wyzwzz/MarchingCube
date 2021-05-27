//
// Created by wyz on 2021/5/27.
//

#include<Render/surface_render.hpp>
#include<Render/line_render.hpp>

namespace mc{
    void SurfaceRenderer::render(const IsoSurface<float>& isosurface) {
        auto vertices=isosurface.getAllVertices();
        vertex_num=vertices.size();
        initGL();
        {
            glGenVertexArrays(1,&vao);
            glBindVertexArray(vao);
            glGenBuffers(1,&vbo);
            glBindBuffer(GL_ARRAY_BUFFER,vbo);
            glBufferData(GL_ARRAY_BUFFER,vertices.size()*sizeof(float)*3,vertices.data(),GL_STATIC_DRAW);
            glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,3*sizeof(float),(void*)0);
            glEnableVertexAttribArray(0);
            glBindVertexArray(0);
        }
        createShader();
        render();
    }

    void SurfaceRenderer::initGL() {
        glfwInit();
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        glfwSwapInterval(1);
        window = glfwCreateWindow(1200, 900, "Surface Render", NULL, NULL);
        if (window == NULL) {
            std::cout << "Failed to create GLFW window" << std::endl;
            glfwTerminate();
            return ;
        }

        glfwMakeContextCurrent(window);

        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
//    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        glfwSwapInterval(1);
        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
            std::cout << "Failed to initialize GLAD" << std::endl;
            glfwTerminate();
            exit(-1);
        }
        glEnable(GL_DEPTH_TEST);
    }

    void SurfaceRenderer::createShader() {
        shader=std::make_unique<mc::Shader>("C:\\Users\\wyz\\projects\\MarchingCubes\\src\\shader\\phone_shading_v.glsl",
                                            "C:\\Users\\wyz\\projects\\MarchingCubes\\src\\shader\\phone_shading_f.glsl");
    }

    void SurfaceRenderer::render() {
        while(!glfwWindowShouldClose(window)){
            glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            glfwPollEvents();

            shader->use();

            glBindVertexArray(vao);
            glDrawArrays(GL_TRIANGLES,0,vertex_num);

            glfwSwapBuffers(window);
        }

    }
}