#pragma once
#include "pch.h"
#include "defs.h"


class CAMERA {
public:
    void step( float elapse, const mouse_t &mouse ) {
        const glm::vec2& mouseD = mouse.mouseD;
        const glm::vec2& wheelD = mouse.wheelD;

        glm::vec3 delta( mouseD.x , -mouseD.y, 0 );

        // right button + mouse :rotation
        if ( mouse.button_stats[2] ) {
            _orbit.x = constrain360( _orbit.x + delta.y * rot_scaler );
            _orbit.y = constrain360( _orbit.y + delta.x * rot_scaler );
        }

        _zoom = glm::clamp( -wheelD.y * zoom_scaler + _zoom, 10.f, 1000.f  );

        glm::mat4  rotation = glm::mat4( glm::rotate( glm::radians( _orbit.y ), Y ) ) 
            * glm::mat4( glm::rotate( glm::radians( _orbit.x ), X ) )  ;

        if ( mouse.button_stats[1] ) {
            _target += glm::mat3(rotation) * delta * pen_scaler ;
        }

        _world = glm::translate( _target ) *  rotation * glm::translate( glm::vec3(0.f,0.f, _zoom) ) ; 

        _view = glm::inverse( _world);

        _proj = glm::perspective( glm::radians(_fov), _aspect, _near, _far );
        _view_proj = _proj * _view;
    }

    void _on_aspect_changed( const float neo_aspect ) {
        this->_aspect = neo_aspect;
    }

    GLFWwindow*     _window = nullptr;

    float           _fov = 60.f;
    float           _aspect = .1f;
    float           _near = 1.0f;
    float           _far = 1000.f;
    float           _zoom = 50.f;

    glm::mat4       _world = glm::mat4(1);
    glm::mat4       _view = glm::mat4(1);
    glm::mat4       _proj = glm::mat4(1);
    glm::mat4       _view_proj = glm::mat4(1);
    glm::vec2       _orbit = {0,0};
    glm::vec3       _target = {0,20, 0};

    float           rot_scaler = 0.25f;
    float           pen_scaler = 0.5f;
    float           zoom_scaler = 10.f;    
};