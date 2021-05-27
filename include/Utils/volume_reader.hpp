//
// Created by wyz on 2021/5/27.
//

#ifndef MARCHINGCUBES_VOLUME_READER_HPP
#define MARCHINGCUBES_VOLUME_READER_HPP
#include<vector>
#include<array>
#include<fstream>
#include<iostream>
namespace mc{

    template<class T>
    class VolumeData{
    public:
        VolumeData()=default;
        using VolumeDataType=typename T;
        std::vector<T> data;
        std::array<uint32_t,3> dim;
    };

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
        in.seekg(std::ios::beg);
        volume_data.data.resize(file_size,0);
        in.read(reinterpret_cast<char*>(volume_data.data.data()),file_size);
        in.close();
        volume_data.dim={(uint32_t)raw_x,(uint32_t)raw_y,(uint32_t)raw_z};
        return volume_data;
    }

}
#endif //MARCHINGCUBES_VOLUME_READER_HPP
