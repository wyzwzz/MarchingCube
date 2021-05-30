#version 430 core
layout(location=0) in vec3 vertexPos;
layout(location=1) in vec3 normal;
uniform mat4 MVPMatrix;
flat out vec3 n;
out vec3 world_pos;
void main() {
    gl_Position=MVPMatrix*vec4(vertexPos,1.f);
    world_pos=vertexPos;
    n=normal;
}
