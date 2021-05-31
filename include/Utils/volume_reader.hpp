//
// Created by wyz on 2021/5/27.
//

#ifndef MARCHINGCUBES_VOLUME_READER_HPP
#define MARCHINGCUBES_VOLUME_READER_HPP
#include<vector>
#include<array>
#include<fstream>
#include<iostream>
#include<limits>
#include<algorithm>
namespace mc{

    template<class T>
    class VolumeData{
    public:
        VolumeData()=default;
        using VolumeDataType=typename T;
        std::vector<T> data;
        std::array<uint32_t,3> dim;
    };

    /**
     * T->uint8_t
     * first convert to 0 - 1.f then multiply 255
     * new_value=(old_value-min_value)/(max_value-min_value) * 255;
     */
    template <class T>
    VolumeData<uint8_t> ConvertToUint8Raw(const VolumeData<T>& volume_data){
        VolumeData<uint8_t> n_volume_data;
        n_volume_data.dim=volume_data.dim;
        n_volume_data.data.assign(volume_data.data.size(),255);
        T min_value,max_value;
        min_value=std::numeric_limits<T>::max();
        max_value=std::numeric_limits<T>::min();
        std::cout<<"max_value for "<<typeid(T).name()<<" is "<<min_value<<", min_value is "<<max_value<<std::endl;
        auto p_min=std::min_element(volume_data.data.cbegin(),volume_data.data.cend());
        auto p_max=std::max_element(volume_data.data.cbegin(),volume_data.data.cend());
        min_value=*p_min;
        max_value=*p_max;
        std::cout<<"volume_data's max_value is "<<max_value<<", min_value is "<<min_value<<std::endl;
        for(size_t i=0;i<volume_data.data.size();i++){
            n_volume_data.data[i]*=1.f*(volume_data.data[i]-min_value)/(max_value-min_value);//may not suit for double
        }
        return n_volume_data;
    }

//    template<class T>
//    void ConvertToUint8Raw(VolumeData<T>& volume_data){
//        if(typeid(T)==typeid(uint8_t)){
//            return;
//        }
//        std::cout<<"but can't convert class VolumeData<"<<typeid(T).name()<<"> to class VolumeData<uint8_t> !"<<std::endl;
//    }


    template<class T>
    VolumeData<T> ReadRawVolume(const char* path,int raw_x,int raw_y,int raw_z){
        VolumeData<T> volume_data;
        size_t assert_size=(size_t)raw_x*raw_y*raw_z*sizeof(T);
        if(assert_size<=0){
            std::cout<<"error assert_size: "<<assert_size<<std::endl;
            return volume_data;
        }
        std::ifstream in(path,std::ios::binary);
        in.seekg(0,std::ios::end);
        size_t file_size=in.tellg();
        if(file_size!=assert_size){
            std::cout<<"assert_size("<<assert_size<<") not equal to file_size("<<file_size<<")"<<std::endl;
            in.close();
            return volume_data;
        }
        in.seekg(0,std::ios::beg);
        volume_data.data.resize(file_size,0);
        in.read(reinterpret_cast<char*>(volume_data.data.data()),file_size);
        in.close();
        volume_data.dim={(uint32_t)raw_x,(uint32_t)raw_y,(uint32_t)raw_z};
        return volume_data;
    }

}
#endif //MARCHINGCUBES_VOLUME_READER_HPP
