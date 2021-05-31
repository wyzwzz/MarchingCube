//
// Created by wyz on 2021/5/29.
//

#ifndef MARCHINGCUBES_CAMERA_HPP
#define MARCHINGCUBES_CAMERA_HPP
#include<glm/glm.hpp>
#include<glm/gtc/matrix_transform.hpp>
namespace mc{
    enum class CameraDefinedKey{
        Forward,
        Backward,
        Left,
        Right,
        Up,
        Bottom
    };
    class Camera{
    public:
        Camera()=default;
        virtual ~Camera(){}
        virtual void processMouseScroll(float yoffset)=0;
        virtual void processMouseMove(float xoffset,float yoffset)=0;
        virtual void processKeyEvent(CameraDefinedKey key,float t)=0;
        virtual glm::mat4 getViewMatrix()=0;
        virtual float getZoom() const =0;
        virtual glm::vec3 getCameraPos() const=0;
    };
    const float PITCH=0.0f;
    const float YAW=-90.0f;
    const float SPEED=50.f;
    const float SENSITIVITY=0.3f;
    const float ZOOM=20.0f;
    class FPSCamera: public Camera{
    public:
        FPSCamera(glm::vec3 pos)
        :pos(pos){
            world_up={0.f,1.f,0.f};

            yaw=YAW;
            pitch=PITCH;
            move_speed=SPEED;
            move_sense=SENSITIVITY;
            zoom=ZOOM;

            updateVector();
        }
        void processMouseScroll(float yoffset) override;
        void processMouseMove(float xoffset,float yoffset) override;
        void processKeyEvent(CameraDefinedKey key,float t) override;
        glm::mat4 getViewMatrix() override;
        glm::vec3 getCameraPos() const override{
            return pos;
        }
        float getZoom()const override{
            return zoom;
        }
    private:
        void updateVector();
    private:
        glm::vec3 pos,view_direction,up,right,world_up;
        float yaw,pitch,move_speed,move_sense,zoom;

    };

    inline void FPSCamera::updateVector() {
        glm::vec3 front;
        front.x=cos(glm::radians(yaw))*cos(glm::radians(pitch));
        front.y=sin(glm::radians(pitch));
        front.z=sin(glm::radians(yaw))*cos(glm::radians(pitch));
        view_direction=glm::normalize(front);
        right=glm::normalize(glm::cross(view_direction,world_up));
        up=glm::normalize(glm::cross(right,view_direction));
    }

    inline void FPSCamera::processMouseScroll(float yoffset) {
        zoom-=yoffset;
        if(zoom<0.1f)
            zoom=0.1f;
        if(zoom>45.0f)
            zoom=45.0f;
    }

    inline void FPSCamera::processMouseMove(float xoffset, float yoffset) {
        xoffset*=move_sense;
        yoffset*=move_sense;

        yaw+=xoffset;
        pitch+=yoffset;


        if (pitch > 60.0f)
            pitch = 60.0f;
        if(pitch<-60.0f)
            pitch=-60.0f;

        updateVector();
    }

    inline void FPSCamera::processKeyEvent(CameraDefinedKey key, float t) {
            float ds=move_speed*t;
            if(key==CameraDefinedKey::Forward)
                pos+=view_direction*ds;
            if(key==CameraDefinedKey::Backward)
                pos-=view_direction*ds;
            if(key==CameraDefinedKey::Left)
                pos-=right*ds;
            if(key==CameraDefinedKey::Right)
                pos+=right*ds;
            if(key==CameraDefinedKey::Up)
                pos+=up*ds;
            if(key==CameraDefinedKey::Bottom)
                pos-=up*ds;

    }

    inline glm::mat4 FPSCamera::getViewMatrix() {
        return glm::lookAt(pos,view_direction+pos,up);
    }

    class TrackingCamera: public Camera{
    public:

    };
}
#endif //MARCHINGCUBES_CAMERA_HPP
