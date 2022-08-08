#pragma once
#include "defs.h"
#include "bounds.hpp"




class KDNODE{
public:
    virtual ~KDNODE() {}
    virtual bool hit( const ray_t &ray, intersection_t &intersection ) const { assert(!"Not implement."); return false;};
    BOUNDS  _bounds  = BOUNDS_CLEAR;
};

class KDTRIANGLE :public KDNODE {
public:
    virtual bool hit( const ray_t &ray, intersection_t &intersection ) const override;


    KDTRIANGLE() {
    }
    KDTRIANGLE( const vertex_t& v0, const vertex_t& v1,const vertex_t& v2   ) {
        _init(v0,v1,v2);
    }
    void    _init(const vertex_t& v0, const vertex_t& v1,const vertex_t& v2 );
    vec3    _intersection_normal(const vec3 &point) const;
    vec3    _points[3];
    vec3    _normals[3];

    vec3    _edge1;
    vec3    _edge2;
    /// for fast barycentric calculation
    float _d00, _d01, _d11, _d;    

};



class KDTREE : public KDNODE {
public:
    enum AXIS {
        AXIS_X, 
        AXIS_Y, 
        AXIS_Z,
        AIXS_NULL = -1
    };
    

    void            build(std::vector<KDNODE *> &objects);
    void            clear();

    virtual bool  hit( const ray_t &ray, intersection_t &intersection ) const override ;

    KDTREE() {

    }
    KDTREE( std::vector<KDNODE *> &objects ) {
        build(objects);
    }

    ~KDTREE() {
        if(_own_children){
            delete _children[0];
            delete _children[1];
        }
   
    }


    AXIS        _split_axis = AIXS_NULL;    /// split axis of this tree
    KDNODE*     _children[2] = {0};
    bool        _own_children =false;
   
};
