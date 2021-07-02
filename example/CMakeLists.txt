add_executable(MarchingCube MarchingCube.cpp)
target_link_libraries(MarchingCube PRIVATE LibMarchingCube)
target_include_directories(MarchingCube
        PRIVATE
        ${PROJECT_SOURCE_DIR}/third_party
        )

add_executable(VolumeRender VolumeRender.cpp)
target_link_libraries(VolumeRender PRIVATE LibMarchingCube)
target_include_directories(VolumeRender
        PRIVATE
        ${PROJECT_SOURCE_DIR}/third_party
)
target_compile_features(VolumeRender PRIVATE cxx_std_17)