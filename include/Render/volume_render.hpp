//
// Created by wyz on 2021/5/31.
//

#ifndef MARCHINGCUBES_VOLUME_RENDER_HPP
#define MARCHINGCUBES_VOLUME_RENDER_HPP

#include<Render/shader_program.hpp>
#include<GLFW/glfw3.h>
#include<Utils/volume_reader.hpp>
#include<memory>

namespace mc {
    class Camera;

    class TransferFunc;

    class VolumeRender final {
    public:
        explicit VolumeRender(int w = 1200, int h = 900);

        ~VolumeRender();

        void setupVolumeData(const VolumeData<uint8_t> &volume_data);

        void setupTransferFunc(TransferFunc &&tf);

        void render();

    private:
        void init();

        void initGL();

        void setupShader();

        void setupCamera();

        void setupProxyCuboid();

        void setupPosFramebuffer();

        void setupScreenQuad();

        void bindShaderUniform();

        void deleteGLResource();

    private:
        std::unique_ptr<mc::Camera> camera;
        std::unique_ptr<mc::Shader> raycast_pos_shader;
        std::unique_ptr<mc::Shader> raycasting_shader;
        GLFWwindow *window;
        int window_w, window_h;

        GLuint volume_tex;
        GLuint tf_tex, ptf_tex;

        std::array<uint32_t, 3> volume_data_dim;
        GLuint proxy_cuboid_vao, proxy_cuboid_vbo, proxy_cuboid_ebo;

        GLuint screen_quad_vao;
        GLuint screen_quad_vbo;

        GLuint raycastpos_fbo;
        GLuint raycastpos_rbo;
        GLuint entrypos_tex;
        GLuint exitpos_tex;
    };


}
#endif //MARCHINGCUBES_VOLUME_RENDER_HPP
