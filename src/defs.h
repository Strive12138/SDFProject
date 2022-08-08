#pragma once
#include "pch.h"



const float FLOAT_EPSILON = std::numeric_limits<float>::epsilon();

const glm::vec3 VEC3_ZERO = {0,0,0};
const glm::vec3 VEC3_EPSILON = {  FLOAT_EPSILON,FLOAT_EPSILON,FLOAT_EPSILON  } ;
const glm::vec3 X = {1,0,0};
const glm::vec3 Y = {0,1,0};
const glm::vec3 Z = {0,0,1};



const float FLOAT_MAX = std::numeric_limits<float>::infinity();
const float PI = 3.14159265359f;
const float RAY_TEST_EPSILON = (1e-4);

inline glm::vec3 max( const glm::vec3& a, const glm::vec3& b) {
    return glm::vec3(fmax(a.x, b.x),
            fmax(a.y, b.y),
            fmax(a.z, b.z));
}

struct vertex_t {
    glm::vec3 pos;
    glm::vec3 nor;
    bool operator==(const vertex_t& other) const {
        return pos == other.pos && nor == other.nor ;
    }
};

struct mouse_t {
    glm::vec2       mouse;
    glm::vec2       mouseD;
    glm::vec2       wheelD;
    std::array<bool, 3> button_stats = {0};

};

struct ray_t {
    vec3 origin;
    vec3 direction;
};

struct intersection_t {
    vec3 point = vec3(0);
    vec3 normal = vec3(0);
    float t = FLOAT_MAX;
};

inline bool compare( const glm::vec3 &a, const glm::vec3 &b ) {
    auto d = glm::abs(a-b);

    for ( int i=0; i<glm::vec3::length(); ++i ) {
        if ( d[i] > FLOAT_EPSILON )  {
            return false;
        }
    }
    return true;
}

inline float constrain360(float x){
    x = fmod(x,360.f);
    if (x < 0.f)
        x += 360.f;
    return x;
}


inline char const* gl_error_string(GLenum const err) noexcept
{
  switch (err)
  {
    // opengl 2 errors (8)
    case GL_NO_ERROR:
      return "GL_NO_ERROR";

    case GL_INVALID_ENUM:
      return "GL_INVALID_ENUM";

    case GL_INVALID_VALUE:
      return "GL_INVALID_VALUE";

    case GL_INVALID_OPERATION:
      return "GL_INVALID_OPERATION";

    case GL_STACK_OVERFLOW:
      return "GL_STACK_OVERFLOW";

    case GL_STACK_UNDERFLOW:
      return "GL_STACK_UNDERFLOW";

    case GL_OUT_OF_MEMORY:
      return "GL_OUT_OF_MEMORY";


    // opengl 3 errors (1)
    case GL_INVALID_FRAMEBUFFER_OPERATION:
      return "GL_INVALID_FRAMEBUFFER_OPERATION";

    // gles 2, 3 and gl 4 error are handled by the switch above
    default:
      assert(!"unknown error");
      return nullptr;
  }
}


#define GL(glfunc) glfunc; { GLenum err =  glGetError();  if (err != GL_NO_ERROR){ printf( "%s:%d %s\n", __FILE__, __LINE__, gl_error_string(err)), exit(-1);  }; };

