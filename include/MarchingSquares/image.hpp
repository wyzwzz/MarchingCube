//
// Created by wyz on 2021/5/13.
//

#ifndef MARCHINGCUBES_IMAGE_HPP
#define MARCHINGCUBES_IMAGE_HPP

#include<vector>
#include<MarchingSquares/isoline.hpp>
#include<Utils/common.hpp>

namespace mc {

    template<class T>
    class Image {
    public:
        Image() = default;

        void setWH(int w, int h);

        void setData(std::vector<T> &&data);

        void setInterpolationMethod(InterpolationMethod method);

        IsoLine<T> getIsoLine(T isovalue) const;

    private:

    private:
        int image_w, image_h;

        InterpolationMethod interpolation_method;

        std::vector<T> image_data;


    };

    template<class T>
    inline void Image<T>::setData(std::vector<T> &&data) {
        image_data = data;
    }

    template<class T>
    inline void Image<T>::setWH(int w, int h) {
        if (w > 0 && h > 0) {
            image_w = w;
            image_h = h;
        }
    }

    template<class T>
    inline void Image<T>::setInterpolationMethod(InterpolationMethod method) {
        this->interpolation_method = method;
    }

    template<class T>
    inline IsoLine<T> Image<T>::getIsoLine(T isovalue) const {
        std::vector<bool> grid_assign;
        assert(w * h == image_data.size());

        grid_assign.resize(image_data.size());
        for (size_t i = 0; i < grid_assign.size(); i++) {
            if (image_data[i] > isovalue) {
                grid_assign[i] = true;
            } else {
                grid_assign[i] = false;
            }
        }


        return IsoLine<T>();
    }


}


#endif //MARCHINGCUBES_IMAGE_HPP
