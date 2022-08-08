#version  460 core
// http://asliceofrendering.com/scene%20helper/2020/01/05/InfiniteGrid/


uniform mat4 MVP_INV;


layout(location = 1) out vec3 nearPoint;
layout(location = 2) out vec3 farPoint;



const vec2 positions[4] = vec2[](
    vec2(-1, -1),
    vec2(+1, -1),
    vec2(-1, +1),
    vec2(+1, +1)
);

vec3 unproj(float x, float y, float z ) {
    vec4 hp = MVP_INV *vec4(x, y, z, 1.0);  
    // hp.y = -hp.y;
    // hp.z = -hp.z;
    return hp.xyz / hp.w;
}

void main() {
    vec3 p = vec3( positions[gl_VertexID], 1 ); 
    nearPoint = unproj(p.x, p.y, 0.0 ).xyz; // unprojecting on the near plane
    farPoint = unproj(p.x, p.y, 1.0 ).xyz; // unprojecting on the far plane
    
    gl_Position = vec4( positions[gl_VertexID], 1, 1.0); // using directly the clipped coordinates
}