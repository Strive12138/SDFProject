#version 460

uniform mat4  MVP;
uniform vec3  CAMERA_POS;
uniform vec3  BOX0;
uniform vec3  BOX1;

uniform sampler3D VOXELS;

layout(location = 1) in vec3 far_point;
layout(location = 0) out vec4 out_color;

const float ESP = 0.1;


float sdf_box(vec3 pos, vec3 hsize) {
  vec3 q = abs(pos) - hsize;
  return length(max(q, 0.0)) + min(max(q.x, max(q.y, q.z)), 0.0);
}

// Definition of signed distance funtion called from
float SDF(vec3 pos) {
  vec3 center = (BOX1 + BOX0) * 0.5;
  vec3 size = (BOX1 - BOX0);
  vec3 hsize = size * 0.5;

  float s =  sdf_box(pos - center, hsize);

  if ( s < ESP ) {


    vec3 uvw = (pos - BOX0) /size ;

    return texture( VOXELS,uvw ).x;

  }
  return s ;

}

void main() {

  
  vec3 D = normalize( far_point - CAMERA_POS ); 
  vec3 P = CAMERA_POS; // the current ray position
  for(int i = 0; i < 30; ++i) {
    float s0 = SDF(P);
    // if(s0 < 0 ) { // ray starting from inside the object
    //   out_color = vec4(1, 0, 0, 1); // paint red
    //   return;
    // }
    if( s0 < ESP ) { // the ray hit the implicit surfacee
      float eps = ESP;
      float sx = SDF(P + vec3(eps, 0, 0)) - s0;
      float sy = SDF(P + vec3(0, eps, 0)) - s0;
      float sz = SDF(P + vec3(0, 0, eps)) - s0;
      vec3 nrm = normalize(vec3(sx, sy, sz)); // normal Dection
      float c = - dot(nrm, D); // Lambersian reflection. The light is at the camera position.
      out_color = vec4(c, c, c, 1);

      vec4 clip_space_pos = MVP * vec4( P.xyz, 1.0);
      clip_space_pos =  (clip_space_pos / clip_space_pos.w);
      gl_FragDepth = clip_space_pos.z;
            
      return;
    }
    P += s0 * D; // advance ray
  }
  discard;
}
