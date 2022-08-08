#pragma once

#include "pch.h"
#include "bounds.hpp"


const int DF_MIN_VOXEL_PER_DIM = 1;         // lower bound: minimum distance field voxels on the shortest axis
const int DF_NUM_SAMPLE_RAYS = 100;         // number of sample rays when sample distance field of mesh
const float DF_DEFAULT_RESOLUTION = 10.0f;  // resolution of distance field volumn, the higher the better, but time consuming


class MODEL;
class BOUNDS;

class SDF
{
public:
    void generate( const MODEL *model, float resolution = DF_DEFAULT_RESOLUTION, int sample_rays = DF_NUM_SAMPLE_RAYS );


    std::string         _source;
    BOUNDS              _bounds;
    float               _resolution;       
    int                 _sample_rays;
    ivec3               _grids;             
    std::vector<float>  _voxels;           
    std::vector<vec3>   _vertices;
    float               _progress = 0;
    float               _duration = 0;

    void                save(const std::string &source);
    bool                load(const std::string &source);    
private:
    void                generate_layer( const MODEL *model, std::vector<vec3> &sampleRays, int zIndex, float &progress);
    friend std::ofstream& operator<<(std::ofstream& o, const SDF& data);
    friend std::ifstream& operator>>(std::ifstream& i, SDF& data);


};

