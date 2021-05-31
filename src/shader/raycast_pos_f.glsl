#version 430 core
uniform uint max_dim;
in vec3 world_pos;
out vec4 frag_color;
void main()
{
    frag_color=vec4(world_pos/max_dim,1.f);
}
