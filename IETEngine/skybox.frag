#version 330 core

in vec3 tex_coord_skybox;
out vec4 color;

uniform samplerCube skybox;

void main()
{    
    color = texture(skybox, tex_coord_skybox);
}