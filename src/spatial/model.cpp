#include "model.hpp"
#include <glm/gtx/hash.hpp>
#include <iostream>

#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>

#include "defs.h"

namespace std {
        template<> struct hash<vertex_t> {
        size_t operator()(vertex_t const& vertex) const {
            return ((hash<glm::vec3>()(vertex.pos) ^ (hash<glm::vec3>()(vertex.nor) << 1)) >> 1);
        }
    };
}

void MODEL::load( const std::string &path  ) {

        tinyobj::attrib_t attrib;
        std::vector<tinyobj::shape_t> shapes;
        std::vector<tinyobj::material_t> materials;
        std::string warn, err;

        if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, path.c_str())) {
            throw std::runtime_error(warn + err);
        }
        std::cout << "Loading Model: "<< path << std::endl;
        _filename = path;
        _bounds.clear();
        
        std::unordered_map<vertex_t, uint32_t> unique_vertices;
        _vertices.clear();
        _indices.clear();

        float S = 1.f;

        for (const auto& shape : shapes) {
            for (const auto& index : shape.mesh.indices) {
                vertex_t vertex{};

                vertex.pos = {
                    attrib.vertices[3 * index.vertex_index + 0] * S,
                    attrib.vertices[3 * index.vertex_index + 1] * S,
                    attrib.vertices[3 * index.vertex_index + 2] * S
                };

                vertex.nor  = {
                    attrib.normals[3 * index.normal_index + 0],
                    attrib.normals[3 * index.normal_index + 1],
                    attrib.normals[3 * index.normal_index + 2]

                };


                if (unique_vertices.count(vertex) == 0) {

                    unique_vertices[vertex] = static_cast<uint32_t>(_vertices.size());
                    _vertices.push_back(vertex);
                    _bounds.add_point( vertex.pos );
                }
                _indices.push_back(unique_vertices[vertex]);
            
            }
        }

        _triangles.resize( _indices.size()/3 );
        for ( size_t i=0;i<_triangles.size(); ++i) {


            auto i0 = _indices[i*3+0];
            auto i1 = _indices[i*3+1];
            auto i2 = _indices[i*3+2];
            _triangles[i]._init( _vertices[i0],_vertices[i1],_vertices[i2] );

        }

        make_primitive( _vertices.data(),  (GLuint)_vertices.size(), _indices.data(),  (GLuint)_indices.size() );
}

void MODEL::make_primitive( const  vertex_t *Vdata, GLuint Vcount, const uint32_t *Idata, GLuint Icount ) {
        
    _primitive.destroy();
    _primitive.ibo_count = Icount;

    // Create VAO
    glGenVertexArrays(1, &_primitive.vao);
    glBindVertexArray( _primitive.vao);

    // create VBO
    glGenBuffers(1, &_primitive.vbo );
    glBindBuffer(GL_ARRAY_BUFFER, _primitive.vbo );

    // copy vertex attribs data to VBO
    GLuint vsize = sizeof(Vdata[0]) * Vcount;
    glBufferData(GL_ARRAY_BUFFER,  vsize, Vdata, GL_STATIC_DRAW); // reserve space

    // Setup vertex attributes

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vdata[0]), 0); 
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vdata[0]), (GLvoid*) long( sizeof( Vdata[0].pos ) ));  
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);        

    // Create index buffer
    glGenBuffers(1, & _primitive.ibo );
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _primitive.ibo );
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, 
        Icount * sizeof(Idata[0]) ,  Idata , GL_STATIC_DRAW);  

    glBindBuffer(GL_ARRAY_BUFFER,0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,0);
    glBindVertexArray(0); 

}   

void MODEL::build_kd_tree() {
    std::vector<KDNODE*> nodes;
    nodes.resize(_triangles.size());
    for ( int i=0; i< _triangles.size(); ++i ) {
        nodes[i] = & _triangles[i];
    }

    _kd_tree.clear();
    _kd_tree.build(nodes);
}



bool MODEL::hit( const ray_t &ray, intersection_t &intersection ) const 
{
    return _kd_tree.hit( ray, intersection );
}