#version  460 core

uniform mat4 MVP;

layout(location = 0) in vec3 position;
layout(location = 1) in float vcolor;
out float fcolor;

void main()
{
    gl_Position = MVP* vec4(position, 1.0);
    gl_PointSize = 5;
    fcolor = vcolor;
}
