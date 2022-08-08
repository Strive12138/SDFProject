#version  460 core

uniform mat4 MVP_INV;


layout(location = 1) out vec3 far_point;



const vec2 positions[4] = vec2[](
    vec2(-1, -1),
    vec2(+1, -1),
    vec2(-1, +1),
    vec2(+1, +1)
);

vec3 unproj(float x, float y, float z ) {
    vec4 hp = MVP_INV *vec4(x, y, z, 1.0);  
    return hp.xyz / hp.w;
}

void main() {
    vec3 p = vec3( positions[gl_VertexID], 1 ); 
    far_point = unproj(p.x, p.y, 1.0 ).xyz; // unprojecting on the far plane
    
    gl_Position = vec4( positions[gl_VertexID], 1, 1.0); // using directly the clipped coordinates
}