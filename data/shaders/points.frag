#version  460 core

in float fcolor;
out vec4 color;

uniform float BBOX_SIZE;

void main()
{   

    color  = fcolor > 0.0 ? vec4(0,1,0,0) * (1-fcolor/BBOX_SIZE)  : vec4(1,0,0,1)  * (-fcolor/BBOX_SIZE) ;
}