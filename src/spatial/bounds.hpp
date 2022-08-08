#pragma once
#include "defs.h"

/* axis-aligned bounding box 
*/
class BOUNDS
{
public:
    inline const vec3& operator [] ( int i ) const {
        assert(i>=0 && i<2);
        return b[i];
    }
    inline vec3& operator [] ( int i ){
        assert(i>=0 && i<2);
        return b[i];
    }   

    inline BOUNDS   operator+( const BOUNDS &a ) const {
        BOUNDS neo;
        neo = *this;
        neo.add_bounds( a );
        return neo;

    }

    inline BOUNDS & operator+=( const BOUNDS &a ) {

        add_bounds( a );
        return *this;
    }

    inline BOUNDS & operator+=( const vec3 &a ) {

        add_point( a );
        return *this;
    }    

    inline void clear()  {
        b[0][0] = b[0][1] = b[0][2] = FLOAT_MAX;
        b[1][0] = b[1][1] = b[1][2] = -FLOAT_MAX;

    }
    inline vec3 center() const
    {
        return (b[0] + b[1]) * .5f;
    }
    inline vec3 hsize() const
    {
        return (b[1] - b[0]) * .5f;
    }

    inline vec3 size() const
    {
        return (b[1] - b[0]);
    }

    inline bool contains(const vec3 &p)
    {
        return (b[0].x < p.x && b[0].y < p.y && b[0].z < p.z) &&
               (b[1].x > p.x && b[1].y > p.y && b[1].z > p.z);
    }
    
    inline bool ray_intersection( const vec3 &start, const vec3 &dir, float *_scale = nullptr ) const {
        int i, ax0, ax1, ax2, side, inside;
        float f, scale;
        vec3 hit;

        ax0 = -1;
        inside = 0;
        for ( i = 0; i < 3; i++ ) {
            if ( start[i] < b[0][i] ) {
                side = 0;
            }
            else if ( start[i] > b[1][i] ) {
                side = 1;
            }
            else {
                inside++;
                continue;
            }
            if ( dir[i] == 0.0f ) {
                continue;
            }
            f = ( start[i] - b[side][i] );
            if ( ax0 < 0 || fabs( f ) > fabs( scale * dir[i] ) ) {
                scale = - ( f / dir[i] );
                ax0 = i;
            }
        }

        if ( ax0 < 0 ) {
            scale = 0.0f;
            if (_scale) *_scale = scale;
            // return true if the start point is inside the bounds
            return ( inside == 3 );
        }

        ax1 = (ax0+1)%3;
        ax2 = (ax0+2)%3;
        hit[ax1] = start[ax1] + scale * dir[ax1];
        hit[ax2] = start[ax2] + scale * dir[ax2];

        if (_scale) *_scale = scale;
        return ( hit[ax1] >= b[0][ax1] && hit[ax1] <= b[1][ax1] &&
                    hit[ax2] >= b[0][ax2] && hit[ax2] <= b[1][ax2] );

    }

    inline bool	add_point( const vec3 &v ){
        bool expanded = false;
        if ( v[0] < b[0][0]) {
            b[0][0] = v[0];
            expanded = true;
        }
        if ( v[0] > b[1][0]) {
            b[1][0] = v[0];
            expanded = true;
        }
        if ( v[1] < b[0][1] ) {
            b[0][1] = v[1];
            expanded = true;
        }
        if ( v[1] > b[1][1]) {
            b[1][1] = v[1];
            expanded = true;
        }
        if ( v[2] < b[0][2] ) {
            b[0][2] = v[2];
            expanded = true;
        }
        if ( v[2] > b[1][2]) {
            b[1][2] = v[2];
            expanded = true;
        }
        return expanded;
    }

    inline bool add_bounds( const BOUNDS &a ) {
        bool expanded = false;
        if ( a.b[0][0] < b[0][0] ) {
            b[0][0] = a.b[0][0];
            expanded = true;
        }
        if ( a.b[0][1] < b[0][1] ) {
            b[0][1] = a.b[0][1];
            expanded = true;
        }
        if ( a.b[0][2] < b[0][2] ) {
            b[0][2] = a.b[0][2];
            expanded = true;
        }
        if ( a.b[1][0] > b[1][0] ) {
            b[1][0] = a.b[1][0];
            expanded = true;
        }
        if ( a.b[1][1] > b[1][1] ) {
            b[1][1] = a.b[1][1];
            expanded = true;
        }
        if ( a.b[1][2] > b[1][2] ) {
            b[1][2] = a.b[1][2];
            expanded = true;
        }
        return expanded;
    }

    explicit inline BOUNDS(void) { }
    explicit inline BOUNDS(vec3 b0, vec3 b1)  { b[0]=b0; b[1]=b1; }
private:
    vec3 b[2];    
};

const BOUNDS BOUNDS_CLEAR = BOUNDS(vec3( FLOAT_MAX,FLOAT_MAX,FLOAT_MAX ),vec3(-FLOAT_MAX,-FLOAT_MAX,-FLOAT_MAX) ); 