#pragma once
#include "pch.h"
#include <unordered_map>



struct SHADER
{
    
    SHADER(const char* vertex_path, const char* fragment_path);
    SHADER(const char* compute_path);
    SHADER(const char* vs_path, const char* tesc_path, const char* tese_path, const char* fs_path);

    ~SHADER();

    void bind() const { glUseProgram(id); }
    void unbind() const { glUseProgram(0); }

    void set_int(const char* name, const GLint value);
    void set_uint(const char* name, const GLuint value);
    void set_float(const char* name, const GLfloat value);
    void set_mat4(const char* name, const GLfloat* value, GLboolean transpose = false);
    void set_vec2(const char* name, const GLfloat x, const GLfloat y);
    void set_vec3(const char* name, const GLfloat x, const GLfloat y, const GLfloat z);
    void set_vec4(const char* name, const GLfloat x, const GLfloat y, const GLfloat z, const GLfloat w);

    GLuint id;

private:
    enum class ShaderType
    {
        Vertex,
        Fragment,
        Compute,
        TessellationControl,
        TessellationEvaluation,
        Program
    };

    GLuint  create_shader(const char* path, ShaderType type) const;
    bool    check_shader_errors(GLuint shader, ShaderType type) const;
    GLint   uniform_location(const char* name);

    std::unordered_map<const char*, GLint> uniform_location_cache;
};


