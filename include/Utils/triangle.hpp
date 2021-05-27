//
// Created by wyz on 2021/5/13.
//

#ifndef MARCHINGCUBES_TRIANGLE_HPP
#define MARCHINGCUBES_TRIANGLE_HPP
#include<Utils/point.hpp>
#include<memory>
template <class T,int dim>
class Triangle{
public:
    std::shared_ptr<Point<T,dim>> point0,point1,point2;
};
template <class T>
using Triangle3D=Triangle<T,3>;
template<class T>
using Triangle2D=Triangle<T,2>;
#endif //MARCHINGCUBES_TRIANGLE_HPP
