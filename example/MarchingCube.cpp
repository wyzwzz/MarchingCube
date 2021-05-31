//
// Created by wyz on 2021/5/31.
//
#include<MarchingCubes/volume.hpp>
#include<Utils/volume_reader.hpp>
#include<Render/surface_render.hpp>
#include<cmdline.hpp>

int main(int argc, char **argv) {

    cmdline::parser cmd;
    cmd.add<std::string>("raw_file", 'i', "input raw volume file", true);
    cmd.add<int>("raw_x", 'x', "raw data x", true);
    cmd.add<int>("raw_y", 'y', "raw data y", true);
    cmd.add<int>("raw_z", 'z', "raw data z", true);
    cmd.add<int>("isovalue", 'v', "iso-surface value", true);

    cmd.parse_check(argc, argv);

    auto raw_file = cmd.get<std::string>("raw_file");
    auto raw_x = cmd.get<int>("raw_x");
    auto raw_y = cmd.get<int>("raw_y");
    auto raw_z = cmd.get<int>("raw_z");
    auto isovalue = cmd.get<int>("isovalue");
    try {
        mc::Volume<uint16_t> volume;
        {
            auto volume_data = mc::ReadRawVolume<uint16_t>(raw_file.c_str(), raw_x, raw_y, raw_z);
            volume.setData(std::move(volume_data.data));
            volume.setVolumeXYZ(volume_data.dim[0], volume_data.dim[1], volume_data.dim[2]);
        }

        START_CPU_TIMER_ONCE
        auto isosurface = volume.getIsoSurface(isovalue);
        END_CPU_TIMER_ONCE

        mc::SurfaceRenderer renderer;

        renderer.render(isosurface);
    }
    catch (const std::exception &err) {
        std::cout << err.what() << std::endl;
        return 1;
    }

    return 0;
}
