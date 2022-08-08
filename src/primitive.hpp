
#pragma once
#include "pch.h"

struct PRIMITIVE {
    struct vtx_t {
        float pos[3];
        float color[3];
    };
    GLuint                      vao = 0;
    GLuint                      vbo = 0;
    GLuint                      ibo = 0;

    GLuint                      vbo_count = 0;
    GLuint                      ibo_count = 0;
    ~PRIMITIVE() {
        assert(!this->vao);
    }

    void bind() {
        glBindVertexArray( vao );
        if (ibo_count)
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,  ibo );
    }

    void unbind() {
        glBindVertexArray( 0 );
        if (ibo_count)
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,  0 );
    }
    
    void create_screen_quad(  ) {
        assert(!this->vao);
        this->ibo_count = 0;
        this->vbo_count = 4;
        
        // Create VAO
        glGenVertexArrays(1, &this->vao);
        glBindVertexArray( this->vao);


        vec2 vertices[] = {
            vec2(1.0f, 1.0f),
            vec2(-1.0f, 1.0f),
            vec2(1.0f,-1.0f),
            vec2(-1.0f, -1.0f)
        };

        vec2 texcoords[] = {
            vec2( 1.0, 1.0 ),
            vec2( 0.0, 1.0 ),
            vec2( 1.0, 0.0 ),
            vec2( 0.0, 0.0 ),
        };

        // create VBO
        glGenBuffers(1, &this->vbo );
        glBindBuffer(GL_ARRAY_BUFFER, this->vbo );

        // copy vertex attribs data to VBO
        GLuint vsize = sizeof(vertices[0]) * 4 * 2 ;
        GLuint csize = sizeof(texcoords[0])* 4 * 2;

        glBufferData(GL_ARRAY_BUFFER,  vsize+csize, 0, GL_STATIC_DRAW); // reserve space
        glBufferSubData(GL_ARRAY_BUFFER, 0, vsize, vertices);       // copy verts at offset 0
        glBufferSubData(GL_ARRAY_BUFFER, vsize, csize, texcoords);    // copy color after verts

        // Setup vertex attributes
    
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(vertices[0])*2, 0); 
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(texcoords[0])*2, (GLvoid*) long(vsize));  
        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(1);  

        glBindBuffer(GL_ARRAY_BUFFER,0);
        glBindVertexArray(0); 

    }

    void create_ponits(const GLfloat *vertices, const GLfloat *colors, GLuint Vcount ) {
        assert(!this->vao);
        this->ibo_count = 0;
        this->vbo_count = Vcount;
        
        // Create VAO
        glGenVertexArrays(1, &this->vao);
        glBindVertexArray( this->vao);


        // create VBO
        glGenBuffers(1, &this->vbo );
        glBindBuffer(GL_ARRAY_BUFFER, this->vbo );

        // copy vertex attribs data to VBO
        GLuint vsize = sizeof(vertices[0]) * Vcount * 3 ;
        GLuint csize = sizeof(colors[0])* Vcount * 1;

        glBufferData(GL_ARRAY_BUFFER,  vsize+csize, 0, GL_STATIC_DRAW); // reserve space
        glBufferSubData(GL_ARRAY_BUFFER, 0, vsize, vertices);       // copy verts at offset 0
        glBufferSubData(GL_ARRAY_BUFFER, vsize, csize, colors);    // copy color after verts

        // Setup vertex attributes
    
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vertices[0])*3, 0); 
        glVertexAttribPointer(1, 1, GL_FLOAT, GL_FALSE, sizeof(colors[0])*1, (GLvoid*) long(vsize));  
        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(1);  

        glBindBuffer(GL_ARRAY_BUFFER,0);
        glBindVertexArray(0); 

    }
    void create_model( const GLfloat *vertices, const GLfloat *colors, GLuint Vcount, const GLushort *indices, GLuint Icount ) {
        assert(!this->vao);

        this->ibo_count = Icount;

          // Create VAO
        glGenVertexArrays(1, &this->vao);
        glBindVertexArray( this->vao);


        // create VBO
        glGenBuffers(1, &this->vbo );
        glBindBuffer(GL_ARRAY_BUFFER, this->vbo );

        // copy vertex attribs data to VBO
        GLuint vsize = sizeof(vertices[0]) * Vcount;
        GLuint csize = sizeof(colors[0])* Vcount;
    

        glBufferData(GL_ARRAY_BUFFER,  vsize+csize, 0, GL_STATIC_DRAW); // reserve space
        glBufferSubData(GL_ARRAY_BUFFER, 0, vsize, vertices);       // copy verts at offset 0
        glBufferSubData(GL_ARRAY_BUFFER, vsize, csize, colors);    // copy color after verts

        
        // Setup vertex attributes
    
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(colors[0])*3, 0); 
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(vertices[0])*3, (GLvoid*) long(vsize));  
        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(1);        

        // Create index buffer
        glGenBuffers(1, &this->ibo );
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->ibo );
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, 
            Icount * sizeof(indices[0]) ,  indices , GL_STATIC_DRAW);  

        glBindBuffer(GL_ARRAY_BUFFER,0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,0);
        glBindVertexArray(0); 
    }
    void destroy( void ) {
        
        glDeleteBuffers(1, &vbo);
        glDeleteBuffers(1, &ibo);
        glDeleteVertexArrays(1, &vao);
        vao = 0;
        ibo = 0;
        vbo = 0;
        ibo_count = 0;
        vbo_count = 0;
    }


};
