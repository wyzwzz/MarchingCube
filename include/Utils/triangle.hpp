//
// Created by wyz on 2021/5/13.
//

#ifndef MARCHINGCUBES_TRIANGLE_HPP
#define MARCHINGCUBES_TRIANGLE_HPP

#include<Utils/point.hpp>
#include<memory>

template<class T, int dim>
class Triangle {
public:
    std::shared_ptr<Point<T, dim>> point0, point1, point2;
};


template<>
class Triangle<float, 3> {
public:
    Triangle(const Point3D<float> &p0, const Point3D<float> &p1, const Point3D<float> &p2) {
        vertices[0] = p0;
        vertices[1] = p1;
        vertices[2] = p2;
    }

public:
    std::array<Point3D<float>, 3> vertices;
};


template<class T>
using Triangle3D = Triangle<T, 3>;
template<class T>
using Triangle2D = Triangle<T, 2>;
#endif //MARCHINGCUBES_TRIANGLE_HPP
