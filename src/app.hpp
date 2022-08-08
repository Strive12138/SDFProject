#pragma once
#include <memory>
#include <iostream>
#include <chrono>
#include <thread>


#include "defs.h"
#include "primitive.hpp"
#include "shader.hpp"
#include "camera.hpp"
#include "spatial/model.hpp"
#include "spatial/sdf.hpp"
#include "texture3d.hpp"



class APP {
private:
    void gui_init( GLFWwindow *window );
    void gui_draw(const glm::vec4& clear_color);
    void gui_quit();

    void app_init_internal();
    void app_quit_internal();
public:

    GLFWwindow* window = nullptr ;

    std::unique_ptr<SHADER> model_prog;
    std::unique_ptr<SHADER> grid_prog;
    std::unique_ptr<SHADER> sdf_prog;
    std::unique_ptr<SHADER> points_prog; 

    PRIMITIVE   screen;
    PRIMITIVE   voxel;
    TEXTURE3D   voxelmap;   

    glm::vec4   clear_color = glm::vec4(0.45f, 0.55f, 0.60f, 1.00f);
   
    CAMERA      camera;
    MODEL       model;
    SDF         sdf;

    mouse_t     mouse;

    bool        show_grids = true;
    bool        show_polygon = false;
    bool        show_color = false;

    bool        show_iso = false;
    bool        show_sdf = true;

    int         triagles = 0;
    char        sdf_progress[255] = {0};
    bool        sdf_ready = false;

    float       accum = 0.f;
    float       last = 0.f;
    float       delta = 0.f;

    glm::ivec2  screen_size;
    
    void app_init(){

        screen.create_screen_quad();

        model_prog.reset ( new SHADER( "data/shaders/default.vert", "data/shaders/default.frag" ) );
        grid_prog.reset( new SHADER( "data/shaders/grid.vert", "data/shaders/grid.frag" ));
        sdf_prog.reset( new SHADER( "data/shaders/sdf.vert", "data/shaders/sdf.frag"));
        points_prog.reset( new SHADER( "data/shaders/points.vert", "data/shaders/points.frag"));

        model.load( "data/models/bunny.obj" );
        triagles = model._triangles.size();

        static std::thread sdf_thread([&]() {
            sdf_ready = false;

            if (!sdf.load( model._filename ) ) 
            {
                strcpy( sdf_progress,  "Building kd-tree...");
                model.build_kd_tree();
                strcpy( sdf_progress,  "Generating voxels...");

                sdf.generate(&model, 50, 128);

                // different voxels
                //for (int i = 10; i <= 64; i++) {
                //    sdf.generate(&model, i, 64);
                //}

                //different rays
                //for (int i = 1; i <= 400; i++) {
                //    sdf.generate(&model, 40.f, i);
                //}

                strcpy( sdf_progress,  "Createing voxel render props...");
                
                sdf.save(model._filename); //save to file
            }
            strcpy( sdf_progress,  "");

            sdf_ready = true;
        } );
        sdf_thread.detach();
    }

    void app_quit() {
        screen.destroy();
        voxel.destroy();
        
        model_prog.release();
        grid_prog.release();

    }
    void app_draw_scane( float elapse ) {

        const auto& view_proj = camera._view_proj;
        glm::mat4 mvp_inv = glm::inverse( view_proj  );

        if ( sdf_ready )
        {   
            if ( voxel.vbo_count == 0) {
                voxel.create_ponits( &sdf._vertices[0].x , &sdf._voxels[0], sdf._vertices.size()  );

                voxelmap.create( sdf._grids, GL_R32F, GL_RED, GL_FLOAT
                    ,GL_LINEAR,GL_LINEAR,GL_CLAMP_TO_BORDER,GL_CLAMP_TO_BORDER,GL_CLAMP_TO_BORDER, sdf._voxels.data());
            }
            if ( show_sdf ) {
                voxelmap.bind();
                sdf_prog->bind();
                screen.bind();

                glm::vec3 cam_pos = glm::vec3( camera._world[3] );

                const glm::vec3 &b0 = sdf._bounds[0];
                const glm::vec3 &b1 = sdf._bounds[1];
                sdf_prog->set_mat4( "MVP", glm::value_ptr( view_proj ) );

                sdf_prog->set_int( "VOXELS", 0 );
                sdf_prog->set_vec3( "BOX0", b0.x, b0.y, b0.z );
                sdf_prog->set_vec3( "BOX1", b1.x, b1.y, b1.z );

                sdf_prog->set_vec3( "CAMERA_POS",cam_pos.x, cam_pos.y, cam_pos.z );
                sdf_prog->set_mat4( "MVP_INV",glm::value_ptr( mvp_inv ) );
        
                glDrawArrays( GL_TRIANGLE_STRIP ,0, 4);
                
                screen.unbind();

                glBindVertexArray(0);
                sdf_prog->unbind();
                voxelmap.unbind();
            }
            if (show_iso) {
                points_prog->bind();
                // Setup transfrom
                points_prog->set_float( "BBOX_SIZE", sdf._bounds.size().length() );
                points_prog->set_mat4( "MVP", glm::value_ptr( view_proj ) );
                voxel.bind();
                glDrawArrays( GL_POINTS ,0, voxel.vbo_count );
                // Restore scane
                voxel.unbind();
                points_prog->unbind();
            }
        }        
        {
            // Draw model
            auto prim = &model._primitive;
            // Bind shader
            model_prog->bind();
            // Setup transfrom
            model_prog->set_mat4( "MVP", glm::value_ptr( view_proj ) );

            prim->bind();
            // Issue draw commands

            if ( show_color ) {
                model_prog->set_vec4( "COLOR", 0,0,0,0 );
                glDrawElements( GL_TRIANGLES , prim->ibo_count, GL_UNSIGNED_INT, (0) );
            }
            if ( show_polygon ) {
                glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
                model_prog->set_vec4( "COLOR", 1,1,1,1 );
                glDrawElements( GL_TRIANGLES , prim->ibo_count, GL_UNSIGNED_INT, (0) );

                glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            }
            // Restore scene
            prim->unbind();
            model_prog->unbind();
        }
        // Draw grids
        if ( show_grids ) {
            grid_prog->bind();
            screen.bind();
            grid_prog->set_mat4( "MVP_INV",glm::value_ptr( mvp_inv ) );
            grid_prog->set_mat4( "MVP", glm::value_ptr( view_proj ) );
            grid_prog->set_vec2( "NF",  camera._near , camera._far);
            grid_prog->set_vec4( "BCOLOR",   clear_color.x,clear_color.y,clear_color.z,clear_color.w );
            glDrawArrays( GL_TRIANGLE_STRIP ,0, 4);
            screen.unbind();
            grid_prog->unbind();
        }
    }

    void init() {
        app_init_internal();
        app_init();
        gui_init( window );
    }

    void quit() {
        gui_quit();
        app_quit();
        app_quit_internal();
    }
    void run();

};
