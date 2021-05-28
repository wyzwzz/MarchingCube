//
// Created by wyz on 2021/5/13.
//

#ifndef MARCHINGCUBES_ISOSURFACE_HPP
#define MARCHINGCUBES_ISOSURFACE_HPP

#include<Utils/triangle.hpp>
#include<vector>
#include<iostream>

namespace mc {
    template<class T>
    class IsoSurface;

    template<>
    class IsoSurface<float> {
    public:
        std::vector<std::array<float, 3>> getAllVertices() const;

        void addTriangle(const Triangle3D<float>& tri);

    private:

        std::vector<Triangle3D<float>> triangles;

        double isovalue;
    };


    inline std::vector<std::array<float, 3>> IsoSurface<float>::getAllVertices() const {
        std::vector<std::array<float, 3>> vertices;
        vertices.reserve(triangles.size() * 3 * sizeof(Point<float, 3>));
        std::cout << "triangles num: " << triangles.size() << std::endl;
        for (auto &triangle:triangles) {
            vertices.push_back(triangle.vertices[0].pos);
            vertices.push_back(triangle.vertices[1].pos);
            vertices.push_back(triangle.vertices[2].pos);
        }
        std::cout << "all vertices size: " << vertices.size() << std::endl;
        return vertices;
    }

    inline void IsoSurface<float>::addTriangle(const Triangle3D<float> &tri) {

    }


}
#endif //MARCHINGCUBES_ISOSURFACE_HPP
