#version  460 core

in vec3 fcolor;
out vec4 color;

uniform vec4 COLOR;


void main()
{
    color = mix( vec4(fcolor*0.5 + 0.5, 1.0), COLOR, COLOR.a );
}