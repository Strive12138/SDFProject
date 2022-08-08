#pragma once
#include <unordered_map>
#include "defs.h"
#include "primitive.hpp"
#include "bounds.hpp"
#include "kdtree.hpp"


class MODEL {
public:
    ~MODEL() {
        _primitive.destroy();
    }
    PRIMITIVE               _primitive;
    BOUNDS                  _bounds;
    std::string             _filename;
    std::vector<vertex_t>   _vertices;
    std::vector<uint32_t>   _indices;
    std::vector<KDTRIANGLE> _triangles;

    KDTREE                  _kd_tree;
    void load( const std::string &path );

    void build_kd_tree();

    bool hit( const ray_t &ray, intersection_t &intersection ) const;

private:
    void make_primitive( const  vertex_t *Vdata, GLuint Vcount, const uint32_t *Idata, GLuint Icount ) ;
};