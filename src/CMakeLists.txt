list(
        APPEND
        MarchingCube_SRCS
        image.cpp
        volume.cpp
        render.cpp
        ${PROJECT_SOURCE_DIR}/third_party/glad/src/glad.c
)
add_library(LibMarchingCube STATIC ${MarchingCube_SRCS})
include(${PROJECT_SOURCE_DIR}/third_party/glfw.cmake)
include(${PROJECT_SOURCE_DIR}/third_party/glm.cmake)
find_package(OpenMP)
target_include_directories(LibMarchingCube
        PUBLIC
        ${PROJECT_SOURCE_DIR}/include
        ${PROJECT_SOURCE_DIR}/third_party/glad/include
        )
target_link_libraries(LibMarchingCube
        PUBLIC
        glfw
        glm::glm
        OpenMP::OpenMP_CXX
        )
target_compile_features(LibMarchingCube PRIVATE cxx_std_11)