#include "kdtree.hpp"
#include <algorithm>
#include <iostream>
#include "math.h"
using namespace std;


bool ray_interset_triangle(  const vertex_t *vtxs[3],
    const vec3 &ray_origin, const vec3 &ray_direction,  float &min_dist, vec3& hit_normal )
{
    const vec3& v0 = (vtxs[0])->pos;
    const vec3& v1 = (vtxs[1])->pos;
    const vec3& v2 = (vtxs[2])->pos;


    vec3 e1 = v1 - v0; // edge 1
    vec3 e2 = v2 - v0; // edge 2
    vec3 b = ray_origin - v0; // Ax = b;
    vec3 d = ray_direction;

    mat3 A(-d, e1, e2);

    // use Cramer's rule instead of inverse of matrix
    float f2 = -d.x * e1.y * e2.z - d.y * e1.z * e2.x - d.z * e1.x * e2.y - (-d.z * e1.y * e2.x - d.y * e1.x * e2.z - d.x * e1.z * e2.y);
    if (f2 != 0)
    {
        // float t = glm::determinant(mat3(b, e1, e2)) / f2;
        float t = b.x * e1.y * e2.z + b.y * e1.z * e2.x + b.z * e1.x * e2.y - (b.z * e1.y * e2.x + b.y * e1.x * e2.z + b.x * e1.z * e2.y);
        t /= f2;
        if (t >= RAY_TEST_EPSILON && t < min_dist)
        {
            // float u = glm::determinant(mat3(-d, b, e2)) / f2;
            float u = -d.x * b.y * e2.z - d.y * b.z * e2.x - d.z * b.x * e2.y - (-d.z * b.y * e2.x - d.y * b.x * e2.z - d.x * b.z * e2.y);
            u /= f2;
            if (u >= 0 && u <= 1.0f)
            {
                // float v = glm::determinant(mat3(-d, e1, b)) / f2;
                float v = -d.x * e1.y * b.z - d.y * e1.z * b.x - d.z * e1.x * b.y - (-d.z * e1.y * b.x - d.y * e1.x * b.z - d.x * e1.z * b.y);
                v /= f2;
                if (v >= 0 && u + v <= 1.0f)
                {
                    min_dist = t;
                    
                    hit_normal = (1.f - u - v) * vtxs[0]->nor +
                                u * vtxs[1]->nor +
                                v * vtxs[2]->nor;
                    return true;

                }
            }
        }
    }

    return false;

}


void KDTRIANGLE::_init( const vertex_t& v0, const vertex_t& v1,const vertex_t& v2   ) 
{

    _points[0] = v0.pos;
    _points[1] = v1.pos;
    _points[2] = v2.pos;

    _normals[0] = v0.nor;
    _normals[1] = v1.nor;
    _normals[2] = v2.nor;    

    _edge1 = _points[1] - _points[0];
    _edge2 = _points[2] - _points[0];
    // for accelerate barycentric calculation
    _d00 = dot(_edge1,_edge1);
    _d01 = dot(_edge1,_edge2);
    _d11 = dot(_edge2,_edge2);

    _d = _d00 * _d11 - _d01 * _d01;

    _bounds.clear();
    _bounds.add_point(_points[0]);
    _bounds.add_point(_points[1]);
    _bounds.add_point(_points[2]);

}

bool KDTRIANGLE::hit(const ray_t &ray, intersection_t &intersection) const {

    // Ref : https://cadxfem.org/inf/Fast%20MinimumStorage%20RayTriangle%20Intersection.pdf
    float t, u, v;
    const vec3 &v0 = _points[0];
    const vec3 &v1 = _points[1];
    const vec3 &v2 = _points[2];
    const vec3 &E1 = _edge1;
    const vec3 &E2 = _edge2;
    vec3 P = glm::cross ( ray.direction, E2);

    float det = glm::dot( E1, P);
    vec3 T;
    if ( det > 0 ) {
        T = ray.origin - v0;
    } else {
        T = v0 - ray.origin;
        det = -det;
    }
    if ( det < RAY_TEST_EPSILON ) {
        return false;
    }
    u = glm::dot(T,P);
    if ( u < 0.0f || u > det ) {
        return false;
    }
    vec3 Q = glm::cross(T,E1);
    v = glm::dot(ray.direction,Q);

    if (v < 0.0f || u + v > det ) {
        return false;
    }
    t = glm::dot(E2,Q);
    float fInvDet = 1.0f / det;
    t *= fInvDet;
    if (t <= intersection.t && t >= RAY_TEST_EPSILON ) {
        intersection.point = ray.origin + ray.direction * t;
        intersection.normal = _intersection_normal(intersection.point);
        intersection.t = t;
        return true;
    } else {
        return false;
    }

}

vec3 KDTRIANGLE::_intersection_normal(const vec3 &p) const {
    vec3 v2_ = p - _points[0];
    float d20 = glm::dot(v2_,_edge1);
    float d21 = glm::dot(v2_,_edge2);
    float v = (_d11 * d20 - _d01 * d21) / _d;
    float w = (_d00 * d21 - _d01 * d20) / _d;
    float u = 1 - v - w;

    vec3 barycentric = vec3(u, v, w);
    barycentric.x = abs(barycentric.x);
    barycentric.y = abs(barycentric.y);
    barycentric.z = abs(barycentric.z);

    vec3 return_normal = glm::normalize( vec3(
                              barycentric.x * (_normals[0]) +
                              barycentric.y * (_normals[1]) +
                              barycentric.z * (_normals[2])) );
    return return_normal;
}

void KDTREE::clear() {
    if(_own_children){
        delete _children[0];
        delete _children[1];
    }
    _children[0]=_children[1] = nullptr;
    _split_axis = AIXS_NULL;
    _own_children = false;
    _bounds.clear();
}

void KDTREE::build(vector<KDNODE *> &objects) {

    BOUNDS objects_aabb = BOUNDS_CLEAR;

    for (auto &object : objects) {
        objects_aabb += object->_bounds;
    }
    vec3 diff = objects_aabb[1] - objects_aabb[0];

    if (diff.x > diff.y && diff.x > diff.z) {
        _split_axis = AXIS_X;
    }else if (diff.y > diff.x && diff.y > diff.z) {
        _split_axis= AXIS_Y;
    }else {
        _split_axis =  AXIS_Z;
    }

    auto length = objects.size();

    if (length == 1) {
        _own_children = false;
        _children[0] = objects[0];
        _children[1] = nullptr;
        _bounds = _children[0]->_bounds;
    } else if (length == 2) {
         _own_children = false;
        _children[0] = objects[0];
        _children[1] = objects[1];
        _bounds = _children[0]->_bounds + _children[1]->_bounds;
    } else {
         _own_children = true;

        vector<KDNODE *> child0;
        vector<KDNODE *> child1;

        float min_val, max_val;
        min_val = FLOAT_MAX;
        max_val = -FLOAT_MAX;

        // get min and max of all objects
        for (unsigned int i = 0; i < objects.size(); i++) {
            BOUNDS bbox = objects[i]->_bounds;
            min_val = min(min_val, bbox[0][_split_axis]);
            max_val = max(max_val, bbox[1][_split_axis]);
        }

        // middle of min and max
        float middle = (max_val + min_val) / 2;

        for (unsigned int i = 0; i < objects.size(); i++) {
            BOUNDS bbox = objects[i]->_bounds;
            float center = bbox.center()[_split_axis];
            if (center < middle) {
                child0.emplace_back(objects[i]);
            } else {
                child1.emplace_back(objects[i]);
            }
        }

        // fix some extreme case when partition failed
        size_t index = 0;
        if (child1.size() == objects.size()) {
            float min_center = FLOAT_MAX;
            for (size_t i = 0; i < child1.size(); i++) {
                BOUNDS bbox = child1[i]->_bounds;
                float center = bbox.center()[_split_axis];
                if (center < min_center) {
                    min_center = center;
                    index = i;
                }
            }
            //std::cout << child1.size() << " " << index << std::endl;

            child0.emplace_back(child1[index]);
            child1.erase(child1.begin() + index);

        } else if (child0.size() == objects.size()) {
            float max_center = -FLOAT_MAX;
            for (unsigned int i = 0; i < child0.size(); i++) {
                BOUNDS bbox = child0[i]->_bounds;
                float center = bbox.center()[_split_axis];
                if (center > max_center) {
                    max_center = center;
                    index = i;
                }
            }
            //std::cout << child0.size() << " " << index << std::endl;
            child1.emplace_back(child0[index]);
            child0.erase(child0.begin() + index);
        }

        _children[0] = child0.size() > 0 ? new KDTREE(child0) : nullptr;
        _children[1] = child1.size() > 0 ? new KDTREE(child1) : nullptr;

        if (_children[0] == nullptr) {
            _bounds = _children[1]->_bounds;
        } else if (_children[1] == nullptr) {
            _bounds = _children[0]->_bounds;
        } else {
            _bounds = _children[0]->_bounds + _children[1]->_bounds;
        }
    }
}




bool KDTREE::hit(const ray_t &ray, intersection_t &intersection) const {
    if (!_bounds.ray_intersection(ray.origin, ray.direction )) {
        return false;
    }

    bool hit = false;

    if (ray.direction[_split_axis] >= 0) {
        if (_children[0] != nullptr) {
            hit |= _children[0]->hit(ray, intersection);
        }
        if (_children[1] != nullptr) {
            hit |= _children[1]->hit(ray, intersection);
        }
    } else {
        if (_children[1] != nullptr) {
            hit |= _children[1]->hit(ray, intersection);
        }
        if (_children[0] != nullptr) {
            hit |= _children[0]->hit(ray, intersection);
        }
    }

    return hit;
}