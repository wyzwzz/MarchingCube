//
// Created by wyz on 2021/5/13.
//

#ifndef MARCHINGCUBES_EDGE_HPP
#define MARCHINGCUBES_EDGE_HPP

#include<Utils/point.hpp>
#include<memory>
template <class T,int dim>
class Edge{
public:
    std::shared_ptr<Point<T,dim>> point0,point1;
};
template <class T>
using Edge2D=Edge<T,2>;
template <class T>
using Edge3D=Edge<T,3>;

#endif //MARCHINGCUBES_EDGE_HPP
