//
// Created by wyz on 2021/5/13.
//

#ifndef MARCHINGCUBES_POINT_HPP
#define MARCHINGCUBES_POINT_HPP
#include<array>
template <class T,int dim>
class Point{
public:
    Point()=default;
    std::array<T,dim> pos;
};

template <>
class Point<float,3>{
public:
    Point()=default;
    std::array<float,3> pos;
//    std::array<uint16_t ,4> idx;
};

template <class T>
using Point2D = Point<T,2> ;
template <class T>
using Point3D = Point<T,3> ;


#endif //MARCHINGCUBES_POINT_HPP
