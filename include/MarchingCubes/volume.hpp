//
// Created by wyz on 2021/5/13.
//

#ifndef MARCHINGCUBES_VOLUME_HPP
#define MARCHINGCUBES_VOLUME_HPP
#include<vector>
#include<Utils/common.hpp>
#include<MarchingCubes/isosurface.hpp>
#include<Utils/lookup_table.hpp>
#include<cassert>
#include<algorithm>
#include<functional>
namespace mc{
    template<class T>
    class Volume{
    public:
        Volume():volume_x(0),volume_y(0),volume_z(0),interpolation_method(InterpolationMethod::Linear){};
        IsoSurface<float> getIsoSurface(T isovalue) const;
        void setVolumeXYZ(int x,int y,int z);
        void setData(std::vector<T>&& data);
        void setInterpolationMethod(InterpolationMethod method);

    private:
        class Node{
        public:
            Node(T v):x(-1),y(-1),z(-1),value(v){}
            Node(int x,int y,int z,T v):x(x),y(y),z(z),value(v){}
            Node(const Node& node){
                *this=node;
            }
            Node& operator=(const Node& node){
                this->x=node.x;
                this->y=node.y;
                this->z=node.z;
                this->value=node.value;
                return *this;
            }
            friend bool operator>(const Node& n1,const Node& n2){
                return n1.value>n2.value;
            }
            int x,y,z;
            T value;
        };
        class Layer{
        public:
            /**
             * x is 0 ~ w-2
             * y is 0 ~ h-2
             */
            std::array<Node,8> getValue(int x,int y) const{
                assert(d_slice.size()==u_slice.size() && d_slice.size()==(size_t)w*h);

                size_t idx0=x+(size_t)y*w;
                size_t idx1=idx0+1;
                size_t idx2=x+(size_t)(y+1)*w;
                size_t idx3=idx2+1;

                return {Node{x,y,d,d_slice[idx0]},Node{x+1,y,d,d_slice[idx1]},
                        Node{x,y+1,d,d_slice[idx2]},Node{x+1,y+1,d,d_slice[idx3]},
                        Node{x,y,d+1,u_slice[idx0]},Node{x+1,y,d+1,u_slice[idx1]},
                        Node{x,y+1,d+1,u_slice[idx2]},Node{x+1,y+1,d+1,u_slice[idx3]}};
            }
            int w,h,d;
            std::vector<T> d_slice,u_slice;
        };
    private:
        std::vector<T> volume_data;

        InterpolationMethod interpolation_method;

        int volume_x,volume_y,volume_z;
    };

    template<class T>
    inline IsoSurface<float> Volume<T>::getIsoSurface(T isovalue) const {
        std::function<Point3D<float>(const Node& n1,const Node& n2,T value)> interpolation;
        if(interpolation_method==InterpolationMethod::MidPoint){
            interpolation=[](const Node& n1,const Node& n2,T value)->Point3D<float>{
                assert((n1.value>value && n2.value<=value) || (n2.value>value && n1.value<=value));
                Point3D<float> p;
                p.pos={((float)n1.x+n2.x)/2,((float)n1.y+n2.y)/2,((float)n1.z+n2.z)/2};
                return p;
            };
        }
        else if(interpolation_method==InterpolationMethod::Linear){
            interpolation=[](const Node& n1,const Node& n2,T value)->Point3D<float>{
                assert((n1.value>value && n2.value<=value) || (n2.value>value && n1.value<=value));
                Point3D<float> p;
                assert(n1.value!=n2.value);
                float a=float(value-n2.value)/(n1.value-n2.value);
                p.pos[0]=a*n1.x+(1.f-a)*n2.x;
                p.pos[1]=a*n1.y+(1.f-a)*n2.y;
                p.pos[2]=a*n1.z+(1.f-a)*n2.z;

                return p;
            };
        }
        IsoSurface<float> iso_surface;
        size_t slice_size=(size_t)volume_x*volume_y;
        Layer layer;
        layer.w=volume_x;
        layer.h=volume_y;
        layer.d_slice=std::vector<T>{};
        layer.u_slice=std::vector<T>{};
        layer.d_slice.resize(slice_size,0);
        layer.u_slice.resize(slice_size,0);
        std::copy(volume_data.begin(),volume_data.begin()+slice_size,layer.u_slice.data());
        for(int z=0;z<volume_z-1;z++){
            layer.d=z;
            std::cout<<z<<std::endl;
//            layer.d_slice=std::move(layer.u_slice);
//            std::copy(layer.u_slice.begin(),layer.u_slice.end(),layer.d_slice.data());
            layer.d_slice.swap(layer.u_slice);
//            assert(layer.u_slice.size()==0);
            std::copy(volume_data.begin()+slice_size*(z+1),volume_data.begin()+slice_size*(z+2),layer.u_slice.data());
            assert(layer.u_slice.size()==layer.d_slice.size());
            for(int y=0;y<volume_y-1;y++){
                for(int x=0;x<volume_x-1;x++){
                    std::array<Node,8> values=layer.getValue(x,y);
                    auto num=std::count_if(values.begin(),values.end(),std::bind2nd(std::greater<Node>(),Node(isovalue)));
                    CubeCode<T,Node> cube_code(isovalue,values);
                    uint8_t code=cube_code.getCode(values);
                    switch (num) {
                        case 0:{
                            //no triangle
                            assert(code==cube_case_0);

                            break;
                        }
                        case 1:{
                            assert(code==cube_case_1);
                            Point3D<float> point0=interpolation(values[6],values[4],isovalue);
                            Point3D<float> point1=interpolation(values[6],values[2],isovalue);
                            Point3D<float> point2=interpolation(values[6],values[7],isovalue);
                            iso_surface.addTriangle(Triangle<float,3>{point0,point1,point2});
                            break;
                        }
                        case 2:{
                            assert(code==cube_case_2a || code==cube_case_2b || code==cube_case_2c);
                            if(code==cube_case_2a){
                                Point3D<float> point0,point1,point2,point3;
                                point0=interpolation(values[5],values[4],isovalue);
                                point1=interpolation(values[5],values[1],isovalue);
                                point2=interpolation(values[7],values[3],isovalue);
                                point3=interpolation(values[7],values[6],isovalue);
                                iso_surface.addTriangle(Triangle<float,3>{point0,point2,point1});
                                iso_surface.addTriangle(Triangle<float,3>{point0,point3,point2});
                            }
                            else if(code==cube_case_2b){
                                Point3D<float> point0=interpolation(values[5],values[1],isovalue);
                                Point3D<float> point1=interpolation(values[5],values[4],isovalue);
                                Point3D<float> point2=interpolation(values[5],values[7],isovalue);
                                Point3D<float> point3=interpolation(values[6],values[2],isovalue);
                                Point3D<float> point4=interpolation(values[6],values[4],isovalue);
                                Point3D<float> point5=interpolation(values[6],values[7],isovalue);
                                iso_surface.addTriangle(Triangle<float,3>{point0,point1,point4});
                                iso_surface.addTriangle(Triangle<float,3>{point0,point4,point3});
                                iso_surface.addTriangle(Triangle<float,3>{point0,point5,point2});
                                iso_surface.addTriangle(Triangle<float,3>{point0,point3,point5});
                            }
                            else if(code==cube_case_2c){
                                Point3D<float> point0=interpolation(values[5],values[1],isovalue);
                                Point3D<float> point1=interpolation(values[5],values[4],isovalue);
                                Point3D<float> point2=interpolation(values[5],values[7],isovalue);
                                Point3D<float> point3=interpolation(values[2],values[0],isovalue);
                                Point3D<float> point4=interpolation(values[2],values[3],isovalue);
                                Point3D<float> point5=interpolation(values[2],values[6],isovalue);
                                iso_surface.addTriangle(Triangle<float,3>{point0,point1,point2});
                                iso_surface.addTriangle(Triangle<float,3>{point3,point4,point5});
                            }
                            break;
                        }
                        case 3:{
                            assert(code==cube_case_3a || code==cube_case_3b || code==cube_case_3c);
                            if(code==cube_case_3a){
                                Point3D<float> point0=interpolation(values[4],values[0],isovalue);
                                Point3D<float> point1=interpolation(values[5],values[1],isovalue);
                                Point3D<float> point2=interpolation(values[6],values[2],isovalue);
                                Point3D<float> point3=interpolation(values[5],values[7],isovalue);
                                Point3D<float> point4=interpolation(values[6],values[7],isovalue);
                                iso_surface.addTriangle(Triangle<float,3>{point0,point2,point1});
                                iso_surface.addTriangle(Triangle<float,3>{point1,point2,point4});
                                iso_surface.addTriangle(Triangle<float,3>{point1,point4,point3});
                            }
                            else if(code==cube_case_3b){
                                Point3D<float> point0=interpolation(values[3],values[2],isovalue);
                                Point3D<float> point1=interpolation(values[3],values[1],isovalue);
                                Point3D<float> point2=interpolation(values[4],values[0],isovalue);
                                Point3D<float> point3=interpolation(values[4],values[5],isovalue);
                                Point3D<float> point4=interpolation(values[4],values[6],isovalue);
                                Point3D<float> point5=interpolation(values[7],values[6],isovalue);
                                Point3D<float> point6=interpolation(values[7],values[5],isovalue);
                                iso_surface.addTriangle(Triangle<float,3>{point0,point1,point2});
                                iso_surface.addTriangle(Triangle<float,3>{point0,point2,point4});
                                iso_surface.addTriangle(Triangle<float,3>{point0,point4,point5});
                                iso_surface.addTriangle(Triangle<float,3>{point1,point3,point2});
                                iso_surface.addTriangle(Triangle<float,3>{point1,point6,point3});
                            }
                            else if(code==cube_case_3c){
                                Point3D<float> point0=interpolation(values[3],values[2],isovalue);
                                Point3D<float> point1=interpolation(values[3],values[1],isovalue);
                                Point3D<float> point2=interpolation(values[5],values[1],isovalue);
                                Point3D<float> point3=interpolation(values[6],values[2],isovalue);
                                Point3D<float> point4=interpolation(values[3],values[7],isovalue);
                                Point3D<float> point5=interpolation(values[5],values[4],isovalue);
                                Point3D<float> point6=interpolation(values[6],values[4],isovalue);
                                Point3D<float> point7=interpolation(values[6],values[7],isovalue);
                                Point3D<float> point8=interpolation(values[5],values[7],isovalue);
                                iso_surface.addTriangle(Triangle<float,3>{point4,point7,point8});
                                iso_surface.addTriangle(Triangle<float,3>{point0,point1,point2});
                                iso_surface.addTriangle(Triangle<float,3>{point0,point2,point5});
                                iso_surface.addTriangle(Triangle<float,3>{point0,point5,point3});
                                iso_surface.addTriangle(Triangle<float,3>{point3,point5,point6});
                            }
                            break;
                        }
                        case 4:{
                            assert(code==cube_case_4a || code==cube_case_4b || code==cube_case_4c
                            ||  code==cube_case_4d || code==cube_case_4e || code==cube_case_4f || code==cube_case_4g);
                            if(code==cube_case_4a){
                                Point3D<float> point0=interpolation(values[4],values[0],isovalue);
                                Point3D<float> point1=interpolation(values[5],values[1],isovalue);
                                Point3D<float> point2=interpolation(values[6],values[2],isovalue);
                                Point3D<float> point3=interpolation(values[7],values[1],isovalue);
                                iso_surface.addTriangle(Triangle<float,3>{point0,point2,point1});
                                iso_surface.addTriangle(Triangle<float,3>{point1,point2,point3});
                            }
                            else if(code==cube_case_4b){
                                Point3D<float> point0=interpolation(values[3],values[2],isovalue);
                                Point3D<float> point1=interpolation(values[3],values[1],isovalue);
                                Point3D<float> point2=interpolation(values[5],values[1],isovalue);
                                Point3D<float> point3=interpolation(values[6],values[2],isovalue);
                                Point3D<float> point4=interpolation(values[5],values[4],isovalue);
                                Point3D<float> point5=interpolation(values[6],values[4],isovalue);
                                iso_surface.addTriangle(Triangle<float,3>{point0,point1,point2});
                                iso_surface.addTriangle(Triangle<float,3>{point0,point2,point4});
                                iso_surface.addTriangle(Triangle<float,3>{point0,point4,point3});
                                iso_surface.addTriangle(Triangle<float,3>{point3,point4,point5});
                            }
                            else if(code==cube_case_4c){
                                Point3D<float> point0=interpolation(values[1],values[0],isovalue);
                                Point3D<float> point1=interpolation(values[2],values[0],isovalue);
                                Point3D<float> point2=interpolation(values[2],values[3],isovalue);
                                Point3D<float> point3=interpolation(values[1],values[3],isovalue);
                                Point3D<float> point4=interpolation(values[5],values[4],isovalue);
                                Point3D<float> point5=interpolation(values[6],values[4],isovalue);
                                Point3D<float> point6=interpolation(values[6],values[7],isovalue);
                                Point3D<float> point7=interpolation(values[5],values[7],isovalue);
                                iso_surface.addTriangle(Triangle<float,3>{point0,point5,point1});
                                iso_surface.addTriangle(Triangle<float,3>{point0,point4,point5});
                                iso_surface.addTriangle(Triangle<float,3>{point3,point2,point6});
                                iso_surface.addTriangle(Triangle<float,3>{point3,point6,point7});
                            }
                            else if(code==cube_case_4d){
                                Point3D<float> point0=interpolation(values[2],values[0],isovalue);
                                Point3D<float> point1=interpolation(values[3],values[1],isovalue);
                                Point3D<float> point2=interpolation(values[5],values[1],isovalue);
                                Point3D<float> point3=interpolation(values[2],values[6],isovalue);
                                Point3D<float> point4=interpolation(values[5],values[4],isovalue);
                                Point3D<float> point5=interpolation(values[7],values[6],isovalue);
                                iso_surface.addTriangle(Triangle<float,3>{point0,point1,point2});
                                iso_surface.addTriangle(Triangle<float,3>{point0,point2,point4});
                                iso_surface.addTriangle(Triangle<float,3>{point0,point4,point3});
                                iso_surface.addTriangle(Triangle<float,3>{point3,point4,point5});
                            }
                            else if(code==cube_case_4e){
                                Point3D<float> point0=interpolation(values[0],values[1],isovalue);
                                Point3D<float> point1=interpolation(values[3],values[1],isovalue);
                                Point3D<float> point2=interpolation(values[0],values[4],isovalue);
                                Point3D<float> point3=interpolation(values[5],values[1],isovalue);
                                Point3D<float> point4=interpolation(values[2],values[6],isovalue);
                                Point3D<float> point5=interpolation(values[3],values[7],isovalue);
                                Point3D<float> point6=interpolation(values[5],values[4],isovalue);
                                Point3D<float> point7=interpolation(values[5],values[7],isovalue);
                                iso_surface.addTriangle(Triangle<float,3>{point0,point1,point3});
                                iso_surface.addTriangle(Triangle<float,3>{point2,point6,point4});
                                iso_surface.addTriangle(Triangle<float,3>{point4,point6,point7});
                                iso_surface.addTriangle(Triangle<float,3>{point4,point7,point5});
                            }
                            else if(code==cube_case_4f){
                                Point3D<float> point0=interpolation(values[1],values[0],isovalue);
                                Point3D<float> point1=interpolation(values[2],values[0],isovalue);
                                Point3D<float> point2=interpolation(values[2],values[3],isovalue);
                                Point3D<float> point3=interpolation(values[1],values[3],isovalue);
                                Point3D<float> point4=interpolation(values[4],values[0],isovalue);
                                Point3D<float> point5=interpolation(values[1],values[5],isovalue);
                                Point3D<float> point6=interpolation(values[2],values[6],isovalue);
                                Point3D<float> point7=interpolation(values[7],values[3],isovalue);
                                Point3D<float> point8=interpolation(values[4],values[5],isovalue);
                                Point3D<float> point9=interpolation(values[4],values[6],isovalue);
                                Point3D<float> point10=interpolation(values[7],values[6],isovalue);
                                Point3D<float> point11=interpolation(values[7],values[5],isovalue);
                                iso_surface.addTriangle(Triangle<float,3>{point0,point4,point1});
                                iso_surface.addTriangle(Triangle<float,3>{point2,point7,point3});
                                iso_surface.addTriangle(Triangle<float,3>{point5,point11,point8});
                                iso_surface.addTriangle(Triangle<float,3>{point6,point9,point10});
                            }
                            else if(code==cube_case_4g){
                                Point3D<float> point0=interpolation(values[2],values[0],isovalue);
                                Point3D<float> point1=interpolation(values[3],values[1],isovalue);
                                Point3D<float> point2=interpolation(values[4],values[0],isovalue);
                                Point3D<float> point3=interpolation(values[3],values[7],isovalue);
                                Point3D<float> point4=interpolation(values[4],values[5],isovalue);
                                Point3D<float> point5=interpolation(values[6],values[7],isovalue);
                                iso_surface.addTriangle(Triangle<float,3>{point0,point1,point2});
                                iso_surface.addTriangle(Triangle<float,3>{point1,point4,point2});
                                iso_surface.addTriangle(Triangle<float,3>{point1,point3,point4});
                                iso_surface.addTriangle(Triangle<float,3>{point3,point5,point4});
                            }
                            break;
                        }
                        case 5:{
                            assert(code==cube_case_5a || code==cube_case_5b || code==cube_case_5c);
                            if(code==cube_case_5a){
                                Point3D<float> point0=interpolation(values[0],values[4],isovalue);
                                Point3D<float> point1=interpolation(values[1],values[5],isovalue);
                                Point3D<float> point2=interpolation(values[2],values[6],isovalue);
                                Point3D<float> point3=interpolation(values[7],values[6],isovalue);
                                Point3D<float> point4=interpolation(values[7],values[5],isovalue);
                                iso_surface.addTriangle(Triangle<float,3>{point0,point3,point2});
                                iso_surface.addTriangle(Triangle<float,3>{point0,point4,point3});
                                iso_surface.addTriangle(Triangle<float,3>{point0,point1,point4});
                            }
                            else if(code==cube_case_5b){
                                Point3D<float> point0=interpolation(values[2],values[3],isovalue);
                                Point3D<float> point1=interpolation(values[1],values[3],isovalue);
                                Point3D<float> point2=interpolation(values[0],values[4],isovalue);
                                Point3D<float> point3=interpolation(values[5],values[4],isovalue);
                                Point3D<float> point4=interpolation(values[6],values[4],isovalue);
                                Point3D<float> point5=interpolation(values[6],values[7],isovalue);
                                Point3D<float> point6=interpolation(values[5],values[7],isovalue);
                                iso_surface.addTriangle(Triangle<float,3>{point2,point3,point4});
                                iso_surface.addTriangle(Triangle<float,3>{point0,point5,point1});
                                iso_surface.addTriangle(Triangle<float,3>{point1,point5,point6});
                            }
                            else if(code==cube_case_5c){
                                Point3D<float> point0=interpolation(values[2],values[3],isovalue);
                                Point3D<float> point1=interpolation(values[1],values[3],isovalue);
                                Point3D<float> point2=interpolation(values[1],values[5],isovalue);
                                Point3D<float> point3=interpolation(values[2],values[6],isovalue);
                                Point3D<float> point4=interpolation(values[4],values[5],isovalue);
                                Point3D<float> point5=interpolation(values[4],values[6],isovalue);
                                Point3D<float> point6=interpolation(values[7],values[6],isovalue);
                                Point3D<float> point7=interpolation(values[7],values[5],isovalue);
                                Point3D<float> point8=interpolation(values[7],values[3],isovalue);
                                iso_surface.addTriangle(Triangle<float,3>{point0,point8,point1});
                                iso_surface.addTriangle(Triangle<float,3>{point2,point7,point4});
                                iso_surface.addTriangle(Triangle<float,3>{point3,point5,point6});
                            }
                            break;
                        }
                        case 6:{
                            assert(code==cube_case_6a || code==cube_case_6b || code==cube_case_6c);
                            if(code==cube_case_6a){
                                Point3D<float> point0=interpolation(values[1],values[5],isovalue);
                                Point3D<float> point1=interpolation(values[3],values[7],isovalue);
                                Point3D<float> point2=interpolation(values[4],values[5],isovalue);
                                Point3D<float> point3=interpolation(values[6],values[7],isovalue);
                                iso_surface.addTriangle(Triangle<float,3>{point0,point1,point2});
                                iso_surface.addTriangle(Triangle<float,3>{point1,point3,point2});
                            }
                            else if(code==cube_case_6b){
                                Point3D<float> point0=interpolation(values[1],values[5],isovalue);
                                Point3D<float> point1=interpolation(values[2],values[6],isovalue);
                                Point3D<float> point2=interpolation(values[4],values[5],isovalue);
                                Point3D<float> point3=interpolation(values[4],values[6],isovalue);
                                Point3D<float> point4=interpolation(values[7],values[6],isovalue);
                                Point3D<float> point5=interpolation(values[7],values[5],isovalue);
                                iso_surface.addTriangle(Triangle<float,3>{point0,point5,point2});
                                iso_surface.addTriangle(Triangle<float,3>{point1,point3,point4});
                            }
                            else if(code==cube_case_6c){
                                Point3D<float> point0=interpolation(values[0],values[2],isovalue);
                                Point3D<float> point1=interpolation(values[3],values[2],isovalue);
                                Point3D<float> point2=interpolation(values[1],values[5],isovalue);
                                Point3D<float> point3=interpolation(values[6],values[2],isovalue);
                                Point3D<float> point4=interpolation(values[4],values[5],isovalue);
                                Point3D<float> point5=interpolation(values[7],values[5],isovalue);
                                iso_surface.addTriangle(Triangle<float,3>{point0,point3,point1});
                                iso_surface.addTriangle(Triangle<float,3>{point2,point5,point4});
                            }
                            break;
                        }
                        case 7:{
                            assert(code==cube_case_7);
                                Point3D<float> point0=interpolation(values[2],values[6],isovalue);
                                Point3D<float> point1=interpolation(values[4],values[6],isovalue);
                                Point3D<float> point2=interpolation(values[7],values[6],isovalue);
                                iso_surface.addTriangle(Triangle<float,3>{point0,point1,point2});
                            break;
                        }
                        case 8:{
                            //no triangle
                            assert(code==cube_case_8);

                            break;
                        }
                    }
                }
            }
        }



        return iso_surface;
    }

    template<class T>
    inline void Volume<T>::setVolumeXYZ(int x, int y, int z) {
        if(x>0 && y>0 && z>0){
            volume_x=x;
            volume_y=y;
            volume_z=z;
        }
    }

    template<class T>
    inline void Volume<T>::setData(std::vector<T> &&data) {
        this->volume_data=std::move(data);
    }

    template<class T>
    inline void Volume<T>::setInterpolationMethod(InterpolationMethod method) {
        this->interpolation_method=method;
    }


}


#endif //MARCHINGCUBES_VOLUME_HPP
