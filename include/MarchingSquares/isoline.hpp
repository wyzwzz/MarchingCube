//
// Created by wyz on 2021/5/13.
//

#ifndef MARCHINGCUBES_ISOLINE_HPP
#define MARCHINGCUBES_ISOLINE_HPP
#include<Utils/edge.hpp>
#include<vector>
namespace mc{


    template <class T>
    class IsoLine{
    public:


    private:
        std::vector<Edge2D<T>> edges;
        T isovalue;
    };
}

#endif //MARCHINGCUBES_ISOLINE_HPP
