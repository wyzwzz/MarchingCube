//
// Created by wyz on 2021/5/27.
//
#include<MarchingCubes/volume.hpp>
#include<Utils/volume_reader.hpp>
#include<Render/surface_render.hpp>
#include<Utils/tool.hpp>
int main(int argc,char** argv)
{
    mc::Volume<uint16_t> volume;
    {
        auto volume_data = mc::ReadRawVolume<uint16_t>("cbct_sample_z=507_y=512_x=512.raw", 512, 512, 507);
        volume.setData(std::move(volume_data.data));
        volume.setVolumeXYZ(volume_data.dim[0], volume_data.dim[1], volume_data.dim[2]);
    }

    auto isosurface=volume.getIsoSurface(1500);

    mc::SurfaceRenderer renderer;

//    renderer.render(isosurface);

    return 0;
}
