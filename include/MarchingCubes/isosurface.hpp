//
// Created by wyz on 2021/5/13.
//

#ifndef MARCHINGCUBES_ISOSURFACE_HPP
#define MARCHINGCUBES_ISOSURFACE_HPP

#include<Utils/triangle.hpp>
#include<vector>
#include<iostream>
#include<glm/glm.hpp>
#include<mutex>
namespace mc {
    template<class T>
    class IsoSurface;

    template<>
    class IsoSurface<float> {
    public:
        IsoSurface()=default;
        IsoSurface<float>(const IsoSurface<float>& surface){
            triangles=surface.triangles;
            isovalue=surface.isovalue;
        }
        std::vector<std::array<float, 3>> getAllVertices() const;

        void addTriangle(const Triangle3D<float>& tri);

        void saveToObj() const;
    private:
        std::mutex mtx;

        std::vector<Triangle3D<float>> triangles;

        double isovalue;
    };


    inline std::vector<std::array<float, 3>> IsoSurface<float>::getAllVertices() const {
        std::vector<std::array<float, 3>> vertices;
        vertices.reserve(triangles.size() * 3 * sizeof(Point<float, 3>));
        std::cout << "triangles num: " << triangles.size() << std::endl;
        for (auto &triangle:triangles) {
            auto v0=triangle.vertices[0].pos;
            auto v1=triangle.vertices[1].pos;
            auto v2=triangle.vertices[2].pos;
            glm::vec3 _v0={v0[0],v0[1],v0[2]};
            glm::vec3 _v1={v1[0],v1[1],v1[2]};
            glm::vec3 _v2={v2[0],v2[1],v2[2]};
            glm::vec3 _n=glm::normalize(glm::cross(_v1-_v0,_v2-_v0));
            std::array<float,3> n={_n.x,_n.y,_n.z};
            vertices.push_back(v0);
            vertices.push_back(n);
            vertices.push_back(v1);
            vertices.push_back(n);
            vertices.push_back(v2);
            vertices.push_back(n);
        }
        std::cout << "all vertices size: " << vertices.size() << std::endl;
        return vertices;
    }

    inline void IsoSurface<float>::addTriangle(const Triangle3D<float> &tri) {
        std::unique_lock<std::mutex> lk(mtx);
        triangles.push_back(tri);
    }


}
#endif //MARCHINGCUBES_ISOSURFACE_HPP
