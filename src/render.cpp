//
// Created by wyz on 2021/5/27.
//

#include<Render/surface_render.hpp>
#include<Render/line_render.hpp>
#include<Render/camera.hpp>
#include<functional>
namespace mc{
    SurfaceRenderer::SurfaceRenderer(int w,int h):window_w(w),window_h(h){
        std::cout<<__FUNCTION__ <<std::endl;
    }
    void SurfaceRenderer::render(const IsoSurface<float>& isosurface) {
        std::cout<<__FUNCTION__ <<std::endl;
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
        std::cout<<__FUNCTION__ <<std::endl;
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
        std::cout<<__FUNCTION__ <<std::endl;
        shader=std::make_unique<mc::Shader>("C:\\Users\\wyz\\projects\\MarchingCubes\\src\\shader\\phone_shading_v.glsl",
                                            "C:\\Users\\wyz\\projects\\MarchingCubes\\src\\shader\\phone_shading_f.glsl");
        shader->use();
        shader->setFloat("ka",0.2f);
        shader->setFloat("kd",0.3f);
        shader->setFloat("shininess",100.f);
        shader->setFloat("ks",0.3f);
        shader->setVec3("light_direction",glm::normalize(glm::vec3(-1.0f,-1.0f,-1.0f)));
    }
    std::function<void(GLFWwindow*,float)> process_input;
    void SurfaceRenderer::render() {
        std::cout<<__FUNCTION__ <<std::endl;
        float last_frame_t=0.f;
        glPolygonMode(GL_FRONT_AND_BACK ,GL_LINE);
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
                                                  0.1f,700.0f);
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
        std::cout<<__FUNCTION__ <<std::endl;
        camera=std::make_unique<mc::FPSCamera>(glm::vec3{250.f,250.f,600.f});

        framebuffer_resize_callback=[&](GLFWwindow *window, int width, int height){

            std::cout<<__FUNCTION__ <<std::endl;
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
        std::cout<<__FUNCTION__ <<std::endl;
    }


}