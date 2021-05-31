//
// Created by wyz on 2021/5/27.
//

#include<Render/surface_render.hpp>
#include<Render/line_render.hpp>
#include<Render/volume_render.hpp>
#include<Render/camera.hpp>
#include<Render/transfer_function.h>
#include<functional>
namespace mc{
    SurfaceRenderer::SurfaceRenderer(int w,int h):window_w(w),window_h(h){
//        std::cout<<__FUNCTION__ <<std::endl;
    }
    void SurfaceRenderer::render(const IsoSurface<float>& isosurface) {
//        std::cout<<__FUNCTION__ <<std::endl;
        auto vertices=isosurface.getAllVertices();
        vertex_num=vertices.size()/2;
        initGL();
        {
            glGenVertexArrays(1,&vao);
            glBindVertexArray(vao);
            glGenBuffers(1,&vbo);
            glBindBuffer(GL_ARRAY_BUFFER,vbo);
            glBufferData(GL_ARRAY_BUFFER,vertices.size()*sizeof(float)*3,vertices.data(),GL_STATIC_DRAW);
            glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,6*sizeof(float),(void*)0);
            glEnableVertexAttribArray(0);
            glVertexAttribPointer(1,3,GL_FLOAT,GL_FALSE,6*sizeof(float),(void*)(3*sizeof(float)));
            glEnableVertexAttribArray(1);
            glBindVertexArray(0);
        }
        createShader();
        setupControl();
        render();
    }

    void SurfaceRenderer::initGL() {
//        std::cout<<__FUNCTION__ <<std::endl;
        glfwInit();
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        glfwSwapInterval(1);
        window = glfwCreateWindow(window_w, window_h, "Surface Render", NULL, NULL);
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
//        std::cout<<__FUNCTION__ <<std::endl;
        shader=std::make_unique<mc::Shader>("C:\\Users\\wyz\\projects\\MarchingCubes\\src\\shader\\phone_shading_v.glsl",
                                            "C:\\Users\\wyz\\projects\\MarchingCubes\\src\\shader\\phone_shading_f.glsl");
        shader->use();
        shader->setFloat("ka",0.3f);
        shader->setFloat("kd",0.7f);
        shader->setFloat("shininess",100.f);
        shader->setFloat("ks",0.6f);
        shader->setVec3("light_direction",glm::normalize(glm::vec3(-1.0f,-1.0f,-1.0f)));
    }

    std::function<void(GLFWwindow*,float)> process_input;
    void SurfaceRenderer::render() {
//        std::cout<<__FUNCTION__ <<std::endl;
        float last_frame_t=0.f;
//        glPolygonMode(GL_FRONT_AND_BACK ,GL_LINE);
//        glEnable(GL_CULL_FACE);
//        glFrontFace(GL_CW);
        while(!glfwWindowShouldClose(window)){
            float current_frame_t=glfwGetTime();
            float delta_t=current_frame_t-last_frame_t;
            last_frame_t=current_frame_t;

            glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            glfwPollEvents();
            process_input(window,delta_t);

            glm::mat4 view=camera->getViewMatrix();

            glm::mat4 projection=glm::perspective(glm::radians(camera->getZoom()),
                                                  (float)window_w/(float)window_h,
                                                  0.1f,1000.0f);
            glm::mat4 mvp=projection*view;

            shader->use();
            shader->setVec3("camera_pos",camera->getCameraPos());
            shader->setMat4("MVPMatrix",mvp);
            glBindVertexArray(vao);
            glDrawArrays(GL_TRIANGLES,0,vertex_num);

            glfwSwapBuffers(window);
        }

    }
    std::function<void(GLFWwindow *window, int width, int height)> framebuffer_resize_callback;
    void glfw_framebuffer_resize_callback(GLFWwindow *window, int width, int height){
        framebuffer_resize_callback(window,width,height);
    }
    std::function<void(GLFWwindow* window, int button, int action, int mods)> mouse_button_callback;
    void glfw_mouse_button_callback(GLFWwindow* window, int button, int action, int mods){
        mouse_button_callback(window,button,action,mods);
    }
    std::function<void(GLFWwindow *window, double xpos, double ypos)> mouse_move_callback;
    void glfw_mouse_move_callback(GLFWwindow *window, double xpos, double ypos){
        mouse_move_callback(window,xpos,ypos);
    }
    std::function<void(GLFWwindow *window, double xoffset, double yoffset)> scroll_callback;
    void glfw_scroll_callback(GLFWwindow *window, double xoffset, double yoffset){
        scroll_callback(window,xoffset,yoffset);
    }
    std::function<void(GLFWwindow *window, int key, int scancode, int action, int mods)> keyboard_callback;
    void glfw_keyboard_callback(GLFWwindow *window, int key, int scancode, int action, int mods){
        keyboard_callback(window,key,scancode,action,mods);
    }
    void SurfaceRenderer::setupControl() {
//        std::cout<<__FUNCTION__ <<std::endl;
        camera=std::make_unique<mc::FPSCamera>(glm::vec3{250.f,250.f,600.f});

        framebuffer_resize_callback=[&](GLFWwindow *window, int width, int height){

//            std::cout<<__FUNCTION__ <<std::endl;
        };

        mouse_move_callback=[&](GLFWwindow *window, double xpos, double ypos){
            static double last_x,last_y;
            static bool first=true;
            if(first){
                first=false;
                last_x=xpos;
                last_y=ypos;
            }
            double delta_x=xpos-last_x;
            double delta_y=last_y-ypos;
            last_x=xpos;
            last_y=ypos;
            if(glfwGetMouseButton(window,GLFW_MOUSE_BUTTON_RIGHT)==GLFW_PRESS){
                camera->processMouseMove(delta_x,delta_y);
            }
        };

        scroll_callback=[&](GLFWwindow *window, double xoffset, double yoffset){
            camera->processMouseScroll(yoffset);
        };

        keyboard_callback=[&](GLFWwindow *window, int key, int scancode, int action, int mods){
            if(key==GLFW_KEY_ESCAPE && action==GLFW_PRESS){
                glfwSetWindowShouldClose(window, true);
            }
        };

        process_input=[&](GLFWwindow *window,float delta_time){
            if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS){
                camera->processKeyEvent(mc::CameraDefinedKey::Forward, delta_time);
            }
            if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS){
                camera->processKeyEvent(mc::CameraDefinedKey::Backward, delta_time);

            }
            if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS){
                camera->processKeyEvent(mc::CameraDefinedKey::Left, delta_time);
            }
            if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS){
                camera->processKeyEvent(mc::CameraDefinedKey::Right, delta_time);
            }
            if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS){
                camera->processKeyEvent(mc::CameraDefinedKey::Up, delta_time);
            }
            if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS){
                camera->processKeyEvent(mc::CameraDefinedKey::Bottom, delta_time);
            }
        };
        glfwSetFramebufferSizeCallback(window,glfw_framebuffer_resize_callback);
//        glfwSetMouseButtonCallback(window,glfw_mouse_button_callback);
        glfwSetCursorPosCallback(window,glfw_mouse_move_callback);
        glfwSetScrollCallback(window,glfw_scroll_callback);
        glfwSetKeyCallback(window,glfw_keyboard_callback);
    }

    SurfaceRenderer::~SurfaceRenderer() {
//        std::cout<<__FUNCTION__ <<std::endl;
    }




    /**
     * class member function definition for VolumeRender
     */

    VolumeRender::VolumeRender(int w, int h)
    :window_w(w),window_h(h)
    {
        initGL();
    }

    VolumeRender::~VolumeRender() {
        deleteGLResource();
    }

    void VolumeRender::setupShader() {
//        std::cout<<__FUNCTION__ <<std::endl;
        this->raycast_pos_shader=std::make_unique<mc::Shader>("C:\\Users\\wyz\\projects\\MarchingCubes\\src\\shader\\raycast_pos_v.glsl",
                                                              "C:\\Users\\wyz\\projects\\MarchingCubes\\src\\shader\\raycast_pos_f.glsl");
        this->raycasting_shader=std::make_unique<mc::Shader>("C:\\Users\\wyz\\projects\\MarchingCubes\\src\\shader\\raycasting_v.glsl",
                                                             "C:\\Users\\wyz\\projects\\MarchingCubes\\src\\shader\\raycasting_f.glsl");
    }

    void VolumeRender::setupVolumeData(const VolumeData<uint8_t> &volume_data) {
        this->volume_data_dim=volume_data.dim;
        glGenTextures(1,&volume_tex);
        glBindTexture(GL_TEXTURE_3D,volume_tex);
        //need to binding texture unit
        glTexParameteri(GL_TEXTURE_3D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
        glTexParameteri(GL_TEXTURE_3D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
        float color[4]={0.f,0.f,0.f,0.f};
        glTexParameteri(GL_TEXTURE_3D,GL_TEXTURE_WRAP_R,GL_CLAMP_TO_BORDER);
        glTexParameteri(GL_TEXTURE_3D,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_BORDER);
        glTexParameteri(GL_TEXTURE_3D,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_BORDER);
        glTexParameterfv(GL_TEXTURE_3D,GL_TEXTURE_BORDER_COLOR,color);
        glPixelStorei(GL_UNPACK_ALIGNMENT,1);
        glTexImage3D(GL_TEXTURE_3D,0,GL_RED,volume_data.dim[0],volume_data.dim[1],volume_data.dim[2],0,GL_RED,GL_UNSIGNED_BYTE,volume_data.data.data());
    }

    void VolumeRender::setupTransferFunc(TransferFunc&& tf) {
        glGenTextures(1,&tf_tex);
        glBindTexture(GL_TEXTURE_1D,tf_tex);

        glTexParameteri(GL_TEXTURE_1D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
        glTexParameteri(GL_TEXTURE_1D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
        glTexParameteri(GL_TEXTURE_1D,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);
        glTexImage1D(GL_TEXTURE_1D,0,GL_RGBA,TF_DIM,0,GL_RGBA,GL_FLOAT,tf.getTransferFunction().data());


        glGenTextures(1,&ptf_tex);
        glBindTexture(GL_TEXTURE_2D,ptf_tex);

        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE);
        glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,TF_DIM,TF_DIM,0,GL_RGBA,GL_FLOAT, tf.getPreIntTransferFunc().data());

    }

    void VolumeRender::bindShaderUniform() {
        glBindTextureUnit(0,tf_tex);
        glBindTextureUnit(1,ptf_tex);
        glBindTextureUnit(2,volume_tex);
        glBindTextureUnit(3,entrypos_tex);
        glBindTextureUnit(4,exitpos_tex);

        raycast_pos_shader->use();

        raycast_pos_shader->setUInt("max_dim",std::max(volume_data_dim[0],std::max(volume_data_dim[1],volume_data_dim[2])));

        raycasting_shader->use();
        raycasting_shader->setInt("transfer_func",0);
        raycasting_shader->setInt("preInt_transferfunc",1);
        raycasting_shader->setInt("volume_data",2);

        raycasting_shader->setFloat("step",0.5f);
        raycasting_shader->setFloat("voxel",1.f);
        raycasting_shader->setVec3("volume_dim",volume_data_dim[0],volume_data_dim[1],volume_data_dim[2]);
        raycasting_shader->setUInt("max_dim",std::max(volume_data_dim[0],std::max(volume_data_dim[1],volume_data_dim[2])));

        raycasting_shader->setFloat("ka",0.5f);
        raycasting_shader->setFloat("kd",0.8f);
        raycasting_shader->setFloat("shininess",100.0f);
        raycasting_shader->setFloat("ks",1.0f);
        raycasting_shader->setVec3("light_direction",glm::normalize(glm::vec3(-1.0f,-1.0f,-1.0f)));



    }

    void VolumeRender::render() {
        init();
        bindShaderUniform();
        float last_frame_t=0.f;
        while(!glfwWindowShouldClose(window)){
            float current_frame_t=glfwGetTime();
            float delta_t=current_frame_t-last_frame_t;
            last_frame_t=current_frame_t;

            glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            glfwPollEvents();
            process_input(window,delta_t);

            glm::mat4 view=camera->getViewMatrix();

            glm::mat4 projection=glm::perspective(glm::radians(camera->getZoom()),
                                                  (float)window_w/(float)window_h,
                                                  1.f,1000.0f);
            glm::mat4 mvp=projection*view;

            raycast_pos_shader->use();
            raycast_pos_shader->setMat4("MVPMatrix",mvp);
            glBindFramebuffer(GL_FRAMEBUFFER,raycastpos_fbo);
            glBindVertexArray(proxy_cuboid_vao);

            glDrawBuffer(GL_COLOR_ATTACHMENT0);
            glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
            glDrawElements(GL_TRIANGLES,36,GL_UNSIGNED_INT,0);

            glEnable(GL_CULL_FACE);
            glFrontFace(GL_CCW);
            glDrawBuffer(GL_COLOR_ATTACHMENT1);
            glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
            glDrawElements(GL_TRIANGLES,36,GL_UNSIGNED_INT,0);
            glDisable(GL_CULL_FACE);
            glBindFramebuffer(GL_FRAMEBUFFER,0);

            raycasting_shader->use();
            auto view_pos=camera->getCameraPos();
            if(view_pos.x>=0.f && view_pos.x<=volume_data_dim[0]
            && view_pos.y>=0.f && view_pos.y<=volume_data_dim[1]
            && view_pos.z>=0.f && view_pos.z<=volume_data_dim[2]){
                raycasting_shader->setBool("inside",true);
                raycasting_shader->setVec3("view_pos",view_pos);
            }
            else{
                raycasting_shader->setBool("inside",false);
            }
            glBindVertexArray(screen_quad_vao);
            glDrawArrays(GL_TRIANGLES,0,6);

            glfwSwapBuffers(window);
        }

    }

    void VolumeRender::setupCamera() {
//        std::cout<<__FUNCTION__ <<std::endl;
        camera=std::make_unique<mc::FPSCamera>(glm::vec3{250.f,250.f,600.f});

        framebuffer_resize_callback=[&](GLFWwindow *window, int width, int height){

//            std::cout<<__FUNCTION__ <<std::endl;
        };

        mouse_move_callback=[&](GLFWwindow *window, double xpos, double ypos){
            static double last_x,last_y;
            static bool first=true;
            if(first){
                first=false;
                last_x=xpos;
                last_y=ypos;
            }
            double delta_x=xpos-last_x;
            double delta_y=last_y-ypos;
            last_x=xpos;
            last_y=ypos;
            if(glfwGetMouseButton(window,GLFW_MOUSE_BUTTON_RIGHT)==GLFW_PRESS){
                camera->processMouseMove(delta_x,delta_y);
            }
        };

        scroll_callback=[&](GLFWwindow *window, double xoffset, double yoffset){
            camera->processMouseScroll(yoffset);
        };

        keyboard_callback=[&](GLFWwindow *window, int key, int scancode, int action, int mods){
            if(key==GLFW_KEY_ESCAPE && action==GLFW_PRESS){
                glfwSetWindowShouldClose(window, true);
            }
        };

        process_input=[&](GLFWwindow *window,float delta_time){
            if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS){
                camera->processKeyEvent(mc::CameraDefinedKey::Forward, delta_time);
            }
            if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS){
                camera->processKeyEvent(mc::CameraDefinedKey::Backward, delta_time);

            }
            if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS){
                camera->processKeyEvent(mc::CameraDefinedKey::Left, delta_time);
            }
            if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS){
                camera->processKeyEvent(mc::CameraDefinedKey::Right, delta_time);
            }
            if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS){
                camera->processKeyEvent(mc::CameraDefinedKey::Up, delta_time);
            }
            if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS){
                camera->processKeyEvent(mc::CameraDefinedKey::Bottom, delta_time);
            }
        };
        glfwSetFramebufferSizeCallback(window,glfw_framebuffer_resize_callback);
//        glfwSetMouseButtonCallback(window,glfw_mouse_button_callback);
        glfwSetCursorPosCallback(window,glfw_mouse_move_callback);
        glfwSetScrollCallback(window,glfw_scroll_callback);
        glfwSetKeyCallback(window,glfw_keyboard_callback);
    }

    void VolumeRender::deleteGLResource() {
        glDeleteTextures(1,&tf_tex);
        glDeleteTextures(1,&ptf_tex);
        glDeleteTextures(1,&volume_tex);
        glDeleteTextures(1,&entrypos_tex);
        glDeleteTextures(1,&exitpos_tex);
        glDeleteVertexArrays(1,&proxy_cuboid_vao);
        glDeleteBuffers(1,&proxy_cuboid_vbo);
        glDeleteBuffers(1,&proxy_cuboid_ebo);
        glDeleteVertexArrays(1,&screen_quad_vao);
        glDeleteBuffers(1,&screen_quad_vbo);
        glDeleteFramebuffers(1,&raycastpos_fbo);
        glDeleteRenderbuffers(1,&raycastpos_rbo);
    }

    void VolumeRender::init() {
        setupShader();
        setupCamera();
        setupProxyCuboid();
        setupScreenQuad();
        setupPosFramebuffer();
    }

    void VolumeRender::initGL() {
//        std::cout<<__FUNCTION__ <<std::endl;
        glfwInit();
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        glfwSwapInterval(1);
        window = glfwCreateWindow(window_w, window_h, "Volume Render", NULL, NULL);
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

    void VolumeRender::setupProxyCuboid() {
//        std::cout<<__FUNCTION__ <<std::endl;
        float x=volume_data_dim[0],y=volume_data_dim[1],z=volume_data_dim[2];
        std::array<std::array<GLfloat,3>,8> proxy_cuboid_vertices;
        proxy_cuboid_vertices[0]={0.f,0.f,0.f};
        proxy_cuboid_vertices[1]={x,0.f,0.f};
        proxy_cuboid_vertices[2]={x,y,0.f};
        proxy_cuboid_vertices[3]={0.f,y,0.f};
        proxy_cuboid_vertices[4]={0.f,0.f,z};
        proxy_cuboid_vertices[5]={x,0.f,z};
        proxy_cuboid_vertices[6]={x,y,z};
        proxy_cuboid_vertices[7]={0.f,y,z};

        std::array<GLuint,36> proxy_cuboid_vertex_indices={0,1,2,0,2,3,
                                   0,4,1,4,5,1,
                                   1,5,6,6,2,1,
                                   6,7,2,7,3,2,
                                   7,4,3,3,4,0,
                                   4,7,6,4,6,5};

        glGenVertexArrays(1,&proxy_cuboid_vao);
        glGenBuffers(1,&proxy_cuboid_vbo);
        glGenBuffers(1,&proxy_cuboid_ebo);
        glBindVertexArray(proxy_cuboid_vao);
        glBindBuffer(GL_ARRAY_BUFFER,proxy_cuboid_vbo);
        glBufferData(GL_ARRAY_BUFFER,sizeof(proxy_cuboid_vertices),proxy_cuboid_vertices.data(),GL_STATIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,proxy_cuboid_ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER,sizeof(proxy_cuboid_vertex_indices),proxy_cuboid_vertex_indices.data(),GL_STATIC_DRAW);
        glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,3*sizeof(float),(void*)0);
        glEnableVertexAttribArray(0);
        glBindVertexArray(0);
    }

    void VolumeRender::setupScreenQuad() {
//        std::cout<<__FUNCTION__ <<std::endl;
        std::array<GLfloat,24> screen_quad_vertices={
                -1.0f,  1.0f,  0.0f, 1.0f,
                -1.0f, -1.0f,  0.0f, 0.0f,
                1.0f, -1.0f,  1.0f, 0.0f,

                -1.0f,  1.0f,  0.0f, 1.0f,
                1.0f, -1.0f,  1.0f, 0.0f,
                1.0f,  1.0f,  1.0f, 1.0f
        };

        glGenVertexArrays(1,&screen_quad_vao);
        glGenBuffers(1,&screen_quad_vbo);
        glBindVertexArray(screen_quad_vao);
        glBindBuffer(GL_ARRAY_BUFFER,screen_quad_vbo);
        glBufferData(GL_ARRAY_BUFFER,sizeof(screen_quad_vertices),screen_quad_vertices.data(),GL_STATIC_DRAW);
        glVertexAttribPointer(0,2,GL_FLOAT,GL_FALSE,4*sizeof(float),(void*)0);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1,2,GL_FLOAT,GL_FALSE,4*sizeof(float),(void*)(2*sizeof(float)));
        glEnableVertexAttribArray(1);
        glBindVertexArray(0);
    }

    void VolumeRender::setupPosFramebuffer() {
//        std::cout<<__FUNCTION__ <<std::endl;
        glGenFramebuffers(1,&raycastpos_fbo);
        glBindFramebuffer(GL_FRAMEBUFFER,raycastpos_fbo);

        glGenTextures(1,&entrypos_tex);
        glBindTexture(GL_TEXTURE_2D,entrypos_tex);
//        glBindTextureUnit(ENTRYPOS_TEXTURE_BINDING,entrypos_tex);
        glTextureStorage2D(entrypos_tex,1,GL_RGBA32F,window_w,window_h);
        glBindImageTexture(0,entrypos_tex,0,GL_FALSE,0,GL_READ_ONLY,GL_RGBA32F);
        glFramebufferTexture2D(GL_FRAMEBUFFER,GL_COLOR_ATTACHMENT0,GL_TEXTURE_2D,entrypos_tex,0);

        glGenRenderbuffers(1,&raycastpos_rbo);
        glBindRenderbuffer(GL_RENDERBUFFER,raycastpos_rbo);
        glRenderbufferStorage(GL_RENDERBUFFER,GL_DEPTH24_STENCIL8,window_w,window_h);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, raycastpos_rbo);

        glGenTextures(1,&exitpos_tex);
        glBindTexture(GL_TEXTURE_2D,exitpos_tex);
//        glBindTextureUnit(EXITPOS_TEXTURE_BINDING,exitpos_tex);
        glTextureStorage2D(exitpos_tex,1,GL_RGBA32F,window_w,window_h);
        glBindImageTexture(1,exitpos_tex,0,GL_FALSE,0,GL_READ_ONLY,GL_RGBA32F);
        glFramebufferTexture2D(GL_FRAMEBUFFER,GL_COLOR_ATTACHMENT1,GL_TEXTURE_2D,exitpos_tex,0);

        if(glCheckFramebufferStatus(GL_FRAMEBUFFER)!=GL_FRAMEBUFFER_COMPLETE){
            throw std::runtime_error("Framebuffer object is not complete!");
        }
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

    }



}