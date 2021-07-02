//
// Created by wyz on 2021/5/31.
//
#include<MarchingCubes/volume.hpp>
#include<Utils/volume_reader.hpp>
#include<Render/volume_render.hpp>
#include<Render/transfer_function.h>
#include<cmdline.hpp>
#include<json.hpp>
#include<fstream>


int main(int argc, char **argv) {

    cmdline::parser cmd;
    cmd.add<std::string>("raw_file", 'i', "input raw file path", true);
    cmd.add<int>("raw_x", 'x', "raw data x", true);
    cmd.add<int>("raw_y", 'y', "raw data y", true);
    cmd.add<int>("raw_z", 'z', "raw data z", true);
    cmd.add<std::string>("raw_data_type",'d',"raw volume data's type",true,"uint8",
                         cmdline::oneof<std::string>("uint8","uint16","int16","uint32","int32"));
    cmd.add<std::string>("tf_file", 't', "transfer function config file", true);
    cmd.parse_check(argc, argv);

    auto raw_file = cmd.get<std::string>("raw_file");
    auto raw_x = cmd.get<int>("raw_x");
    auto raw_y = cmd.get<int>("raw_y");
    auto raw_z = cmd.get<int>("raw_z");
    auto voxel_type=cmd.get<std::string>("raw_data_type");
    auto tf_file = cmd.get<std::string>("tf_file");


    try {

        mc::VolumeRender volume_render;
        if(voxel_type=="uint8"){
            auto volume_data = mc::ReadRawVolume<uint8_t>(raw_file.c_str(), raw_x, raw_y, raw_z);
            volume_render.setupVolumeData(mc::ConvertToUint8Raw(volume_data));
        }
        else if(voxel_type=="uint16"){
            auto volume_data = mc::ReadRawVolume<uint16_t>(raw_file.c_str(), raw_x, raw_y, raw_z);
            volume_render.setupVolumeData(mc::ConvertToUint8Raw(volume_data));
        }
        else if(voxel_type=="int16"){
            auto volume_data = mc::ReadRawVolume<int16_t>(raw_file.c_str(), raw_x, raw_y, raw_z);
            volume_render.setupVolumeData(mc::ConvertToUint8Raw(volume_data));
        }
        else if(voxel_type=="uint32"){
            auto volume_data = mc::ReadRawVolume<uint32_t>(raw_file.c_str(), raw_x, raw_y, raw_z);
            volume_render.setupVolumeData(mc::ConvertToUint8Raw(volume_data));
        }
        else if(voxel_type=="int32"){
            auto volume_data = mc::ReadRawVolume<int32_t>(raw_file.c_str(), raw_x, raw_y, raw_z);
            volume_render.setupVolumeData(mc::ConvertToUint8Raw(volume_data));
        }

        std::map<uint8_t, std::array<float, 4>> color_map;

        {
            using json = nlohmann::json;
            std::ifstream in(tf_file);
            if(!in.is_open()){
                throw std::runtime_error("can't open tf config file!");
            }
            json j;
            in>>j;
            if(j.find("tf")!=j.end()){
                auto points=j["tf"];
                for(auto it=points.begin();it!=points.end();it++){
                    int key=std::stoi(it.key());
                    auto values=it.value();
                    color_map[key]={values[0],values[1],values[2],values[3]};
                }
            }
            else{
                throw std::runtime_error("error tf config file!");
            }
        }

        mc::TransferFunc tf(color_map);

        volume_render.setupTransferFunc(std::move(tf));

        volume_render.render();
    }
    catch (const std::exception &err) {

        std::cout << err.what() << std::endl;

        return 1;
    }

    return 0;
}