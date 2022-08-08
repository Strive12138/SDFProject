#pragma once

#include "pch.h"


class TEXTURE3D
{
public:

    void create( const ivec3 &size, GLint internal_format_, GLenum format, GLenum type,
        GLint min_filter = GL_NEAREST, GLint max_filter = GL_NEAREST, GLint wrap_r = GL_CLAMP_TO_BORDER,
        GLint wrap_s = GL_CLAMP_TO_BORDER,GLint wrap_t = GL_CLAMP_TO_BORDER, const GLvoid* data = 0)
    {

        assert(_id==GLuint(-1));

        _size = size;
        _internal_format = internal_format_;
        
        glGenTextures(1, &_id);

        bind();

        glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, min_filter);
        glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, max_filter);
        glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, wrap_r);
        glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, wrap_s);
        glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, wrap_t);

        glTexImage3D(GL_TEXTURE_3D, 0, _internal_format, size.x, size.y, size.z, 0, format, type, data);

        unbind();
    }

    void destroy() {

        glDeleteTextures(1, &_id);

        _id = GLuint(-1);

    }

    inline void bind(const unsigned int unit = 0) const
    {
        glActiveTexture(GL_TEXTURE0 + unit);
        glBindTexture(GL_TEXTURE_3D, _id);
    }

    inline void unbind() const { 
        glBindTexture(GL_TEXTURE_3D, 0);
    }

    ~TEXTURE3D() {
        destroy();
    }
    GLuint      _id = GLuint(-1);
    ivec3       _size = ivec3(0);
    GLint       _internal_format = 0;
};
