#version  460 core
// http://asliceofrendering.com/scene%20helper/2020/01/05/InfiniteGrid/

uniform mat4 MVP;
uniform vec2 NF;
uniform vec4 BCOLOR;


layout(location = 1) in vec3 nearPoint;
layout(location = 2) in vec3 farPoint;
layout(location = 0) out vec4 outColor;


vec4 grid(vec3 fragPos3D, float scale) {
    vec2 coord = fragPos3D.xz * scale; // use the scale variable to set the distance between the lines
    vec2 derivative = fwidth(coord);
    vec2 grid = abs(fract(coord - 0.5) - 0.5) / derivative;
    float line = min(grid.x, grid.y);
    float minimumz = min(derivative.y, 1);
    float minimumx = min(derivative.x, 1);
    vec4 color = vec4( 0.25,0.25,0.25, 1.0 - min(line, 1.0));


    float bs = 5;

    // z axis
    if(fragPos3D.x > -bs * minimumx && fragPos3D.x < bs * minimumx)
        color.z = 1.0;
    // x axis
    if(fragPos3D.z > -bs * minimumz && fragPos3D.z < bs * minimumz)
        color.x = 1.0;

    if (color.w<=0 ) {
        discard;
    }
    return color * color.w;
}


float compute_linear_depth(vec3 pos) {
    vec4 clip_space_pos =  MVP * vec4(pos.xyz, 1.0);
    float clip_space_depth = (clip_space_pos.z / clip_space_pos.w) * 2.0 - 1.0; // put back between -1 and 1
    float near = NF[0];
    float far = NF[1];

    float linear_depth = (2.0 * near * far) / (far + near - clip_space_depth * (far - near)); // get linear value between 0.01 and 100
    return linear_depth / far; // normalize
}

void main() {
    float t = -nearPoint.y / (farPoint.y - nearPoint.y);
    if ( t<=0)
        discard;

    vec3 frag_pos = nearPoint + t * (farPoint - nearPoint);

    vec4 clip_space_pos = MVP * vec4(frag_pos.xyz, 1.0);
    clip_space_pos =  (clip_space_pos / clip_space_pos.w);

    //gl_FragDepth = clip_space_pos.z;

    float linear_depth = compute_linear_depth(frag_pos);
    float fading = max(0, (0.5 - linear_depth));


    outColor = mix ( BCOLOR, grid( frag_pos , 0.1 ), fading) ;
}