#version  460 core

uniform mat4 MVP;

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 vcolor;
out vec3 fcolor;

void main()
{
    gl_Position = MVP * vec4(position, 1.0);
    fcolor = vcolor;
}
