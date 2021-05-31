//
// Created by wyz on 2021/5/31.
//

#include<MarchingCubes/volume.hpp>
#include<Utils/volume_reader.hpp>
#include<Render/volume_render.hpp>
#include<Render/transfer_function.h>
int main(){

    auto volume_data = mc::ReadRawVolume<uint16_t>("cbct_sample_z=507_y=512_x=512.raw", 512, 512, 507);

    mc::VolumeRender volume_render;

    volume_render.setupVolumeData(mc::ConvertToUint8Raw(volume_data));

    {
        std::map<uint8_t ,std::array<float,4>> color_map;

        color_map[0]={0.0f,0.1,0.6,0.0};
        color_map[120]={0.0f,0.f,0.f,0.f};
        color_map[121]={0.2f,0.9f,0.1f,0.8f};
        color_map[255]={0.2f,0.9f,0.1f,0.8f};
        mc::TransferFunc tf(color_map);

        volume_render.setupTransferFunc(std::move(tf));
    }


    volume_render.render();

    return 0;
}