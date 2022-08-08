#pragma once

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glad/glad.h>

#ifdef _WIN32

#elif defined(__APPLE__) || defined(__APPLE_CC__)
    #define GL_GLEXT_LEGACY
    #include <OpenGL/gl.h>
#else // GLX
    #include <GL/glx.h>
#endif


#define GLM_FORCE_DEPTH_ZERO_TO_ONE

#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>

#include <vector>
#include <array>
#include <cassert>
#include <string>
#include <limits>


typedef glm::vec3 vec3;
typedef glm::ivec3 ivec3;
typedef glm::vec2 vec2;

typedef glm::mat3 mat3;
typedef glm::mat4 mat4;