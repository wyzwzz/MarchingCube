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
        class Layer{
        public:
            /**
             * x is 0 ~ w-2
             * y is 0 ~ h-2
             */
            std::array<T,8> getValue(size_t x,size_t y) const{
                assert(d_slice.size()==u_slice.size() && d_slice==(size_t)w*h);

                size_t idx0=x+y*w;
                size_t idx1=idx0+1;
                size_t idx2=x+(y+1)*w;
                size_t idx3=idx2+1;

                return {d_slice[idx0],d_slice[idx1],d_slice[idx2],d_slice[idx3],
                        u_slice[idx0],u_slice[idx1],u_slice[idx2],u_slice[idx3]};
            }
            int w,h;
            std::vector<T> d_slice,u_slice;
        };
    private:
        std::vector<T> volume_data;

        InterpolationMethod interpolation_method;

        int volume_x,volume_y,volume_z;
    };

    template<class T>
    inline IsoSurface<float> Volume<T>::getIsoSurface(T isovalue) const {
        size_t slice_size=(size_t)volume_x*volume_y;
        Layer layer;
        layer.w=volume_x;
        layer.h=volume_y;
        layer.d_slice.resize(slice_size,0);
        layer.u_slice.resize(slice_size,0);
        std::copy(layer.u_slice.begin(),layer.u_slice.end(),volume_data.data());
        for(int z=0;z<volume_z-1;z++){
            layer.d_slice=std::move(layer.u_slice);
            assert(layer.u_slice.size()==0);
            layer.u_slice=std::copy(layer.u_slice.begin(),layer.d_slice.end(),volume_data.data()+slice_size*(z+1));
            for(int y=0;y<volume_y;y++){
                for(int x=0;x<volume_x;x++){

                }
            }
        }



        return IsoSurface<float>();
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
