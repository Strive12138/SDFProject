#include "shader.hpp"
#include "defs.h"

#include <iostream>
#include <fstream>
#include <sstream>

#include <cassert>



SHADER::SHADER(const char* vertex_path, const char* fragment_path)
{
    GLuint vertex_shader = create_shader(vertex_path, ShaderType::Vertex);
    GLuint fragment_shader = create_shader(fragment_path, ShaderType::Fragment);

    id = GL( glCreateProgram());
    GL( glAttachShader(id, vertex_shader) );
    GL( glAttachShader(id, fragment_shader) );
    GL( glLinkProgram(id) );

    if (check_shader_errors(id, ShaderType::Program)) exit(1);

    GL( glDeleteShader(vertex_shader) );
    GL( glDeleteShader(fragment_shader) );
}

SHADER::SHADER(const char* compute_path)
{
    GLuint compute_shader = create_shader(compute_path, ShaderType::Compute);

    id = glCreateProgram();
    glAttachShader(id, compute_shader);
    glLinkProgram(id);

    if (check_shader_errors(id, ShaderType::Program)) exit(1);

    glDeleteShader(compute_shader);
}

SHADER::SHADER(const char* vs_path, const char* tesc_path, const char* tese_path, const char* fs_path)
{
    GLuint vs = create_shader(vs_path, ShaderType::Vertex);
    GLuint tcs = create_shader(tesc_path, ShaderType::TessellationControl);
    GLuint tes = create_shader(tese_path, ShaderType::TessellationEvaluation);
    GLuint fs = create_shader(fs_path, ShaderType::Fragment);

    id = glCreateProgram();
    glAttachShader(id, vs);
    glAttachShader(id, tcs);
    glAttachShader(id, tes);
    glAttachShader(id, fs);
    glLinkProgram(id);

    if (check_shader_errors(id, ShaderType::Program)) exit(1);

    glDeleteShader(vs);
    glDeleteShader(tcs);
    glDeleteShader(tes);
    glDeleteShader(fs);
}

SHADER::~SHADER()
{
    glDeleteProgram(id);
}

void SHADER::set_int(const char* name, const GLint value)
{
    GLint location = uniform_location(name);
    glUniform1i(location, value);
}

void SHADER::set_uint(const char* name, const GLuint value)
{
    GLint location = uniform_location(name);
    glUniform1ui(location, value);
}

void SHADER::set_float(const char* name, const GLfloat value)
{
    GLint location = uniform_location(name);
    glUniform1f(location, value);
}

void SHADER::set_mat4(const char* name, const GLfloat* value, GLboolean transpose)
{
    GLint location = uniform_location(name);
    glUniformMatrix4fv(location, 1, transpose, value);
}

void SHADER::set_vec2(const char* name, const GLfloat x, const GLfloat y)
{
    GLint location = uniform_location(name);
    glUniform2f(location, x, y);
}

void SHADER::set_vec3(const char* name, const GLfloat x, const GLfloat y, const GLfloat z)
{
    GLint location = uniform_location(name);
    glUniform3f(location, x, y, z);
}


void SHADER::set_vec4(const char* name, const GLfloat x, const GLfloat y, const GLfloat z, const GLfloat w) {
    GLint location = uniform_location(name);
    glUniform4f(location, x, y, z, w);
}

GLuint SHADER::create_shader(const char* path, ShaderType type) const
{
    printf (  "Compiling SHADER: %s\n",  path );
    std::ifstream file(path);
    if (file)
    {
        std::stringstream shader_stream;
        shader_stream << file.rdbuf();
        file.close();

        const std::string& shader_source = shader_stream.str();
        const char* shader_source_c_str = shader_source.c_str();

        GLenum shader_type = GL_VERTEX_SHADER;
        switch (type)
        {
            case ShaderType::Fragment:
                shader_type = GL_FRAGMENT_SHADER;
                break;
            case ShaderType::Compute:
                shader_type = GL_COMPUTE_SHADER;
                break;
            case ShaderType::TessellationControl:
                shader_type = GL_TESS_CONTROL_SHADER;
                break;
            case ShaderType::TessellationEvaluation:
                shader_type = GL_TESS_EVALUATION_SHADER;
                break;
        }

        GLuint SHADER = GL( glCreateShader(shader_type) );
        GL( glShaderSource(SHADER, 1, &shader_source_c_str, 0) );
        GL( glCompileShader(SHADER) );

        if (check_shader_errors(SHADER, type)) 
            return GLuint(-1);

        return SHADER;
    }
    return GLuint(-1);
}

bool SHADER::check_shader_errors(GLuint SHADER, ShaderType type) const
{
    GLint success;
    if (type == ShaderType::Program)
    {
        glGetProgramiv(SHADER, GL_LINK_STATUS, &success);
        if (!success)
        {
            GLchar info_log[512];
            glGetProgramInfoLog(SHADER, 512, nullptr, info_log);

            std::cout << "Failed to link SHADER program.\n" << info_log << std::endl;
            return true;
        }
    }
    else
    {
        glGetShaderiv(SHADER, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            GLchar info_log[512];
            glGetShaderInfoLog(SHADER, 512, nullptr, info_log);

            std::cout << info_log << std::endl;

            return true;
        }
    }

    return false;
}

GLint SHADER::uniform_location(const char* name)
{
    if (uniform_location_cache.find(name) != uniform_location_cache.end())
        return uniform_location_cache[name];

    GLint location = glGetUniformLocation(id, name);
    if (location == -1)
        std::cout << "Warning: " << name << " uniform doesn't exist!" << std::endl;

    uniform_location_cache[name] = location;
    return location;
}

