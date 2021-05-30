#version 430 core

flat in vec3 n;
in vec3 world_pos;

uniform float ka;
uniform float kd;
uniform float shininess;
uniform float ks;

uniform vec3 light_direction;
uniform vec3 camera_pos;
out vec4 frag_color;
void main() {
    vec4 diffuse_color=vec4(0.1f,0.9f,0.f,1.f);
    vec3 L=normalize(world_pos-camera_pos);
    vec3 R=-light_direction;//-ray_direction;
    vec3 N=-n;

    vec3 ambient=ka*diffuse_color.rgb;
    vec3 specular=ks*pow(max(dot(N,(L+R)/2.0),0.0),shininess)*vec3(1.0f);
    vec3 diffuse=kd*max(dot(N,L),0.0)*diffuse_color.rgb;
    frag_color.rgb=ambient+specular+diffuse;
    frag_color.a=1.f;
}
