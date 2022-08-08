#include <glm/gtx/string_cast.hpp>
#include <fstream>
#include <iostream>
#include <execution>
#include <thread>
#include <future>
#include "sdf.hpp"
#include "defs.h"
#include "model.hpp"


using namespace std;
using namespace glm;


ofstream &operator<<(ofstream &o, const SDF &data)
{
    
    o << data._source << endl
      << data._grids.x << " " << data._grids.y << " " << data._grids.z << endl
      << data._resolution << endl;

    vec3 v = data._bounds[0];
    o << v.x << " " << v.y << " " << v.z << endl;

    v = data._bounds[1];
    o << v.x << " " << v.y << " " << v.z << endl;

    for (float f : data._voxels) //distance values
    {
        o << f << endl;
    }

    for (auto v : data._vertices) // sample voxels
    {
        o << v.x << " " << v.y << " " << v.z << endl;
    }

    return o;
}

ifstream &operator>>(ifstream &i, SDF &data)
{
    data._duration = 0.f;
    data._progress = 0.f;

    int x, y, z;
    float fx, fy, fz;
    float gx, gy, gz;

    i >> data._source;

    i >> x >> y >> z;
    data._grids = ivec3(x, y, z);

    i >> data._resolution;

    i >> fx >> fy >> fz >> gx >> gy >> gz;
    data._bounds = BOUNDS(vec3(fx, fy, fz), vec3(gx, gy, gz));


    int size = data._grids.x * data._grids.y * data._grids.z;
    data._voxels = vector<float>(size);
    data._vertices = vector<vec3>(size);

    for (int idx = 0; idx < size; idx++)
    {
        i >> data._voxels.at(idx);
    }

    for (int idx = 0; idx < size; idx++)
    {
        i >> fx >> fy >> fz;
        data._vertices.at(idx) = vec3(fx, fy, fz);
    }    

    data._progress = 1.f;


    return i;
}

void SDF::generate( const MODEL *model, float resolution, int sample_rays  )
{
    this->_resolution = resolution;
    this->_sample_rays = sample_rays;

    // at least (?) voxels on each dimension
    int minVoxelPerDim = (int)trunc(DF_MIN_VOXEL_PER_DIM * _resolution);

    // new bounding box : bit larger
    BOUNDS originalBox = model->_bounds;
    vec3 originalDiag = originalBox.hsize();
    // new diagonal length, guarantee that the bounding box of distance field volume is LARGER than the bbox of model
    // how much larger? depends on the method to handle sdf query on bbox borders,
    // in this case it should be several-voxels-larger.
    vec3 newDiag = max(originalDiag * 0.2f, 4.f * originalBox.size() / (float)minVoxelPerDim);
    _bounds = BOUNDS(originalBox[0] - newDiag, originalBox[1] + newDiag);

#if !defined NDEBUG
    cout << "Original Box: \t" << glm::to_string(originalBox[0]) << " \t " << glm::to_string(originalBox[1]) << endl;
    cout << glm::to_string(originalBox.center()) << endl;
    cout << "New D F  Box: \t" << glm::to_string(_bounds[0]) << " \t " << glm::to_string(_bounds[1]) << endl;
    cout << glm::to_string(_bounds.center()) << endl;
#endif // DEBUG

    // voxel should be like a cube, having at least certain amount on shortest axis
    vec3 dfBoxSize = _bounds.size();
    float voxelSideLength = fmin(fmin(dfBoxSize.x, dfBoxSize.y), dfBoxSize.z) / minVoxelPerDim;

    vec3 dfDimensions = dfBoxSize / voxelSideLength;
    _grids = ivec3((int)trunc(dfDimensions.x),
                 (int)trunc(dfDimensions.y),
                 (int)trunc(dfDimensions.z));

    // sample rays
    int thetaSteps = (int)std::trunc(sqrt(_sample_rays * 2));
    int phiSteps = thetaSteps / 2;

    vector<vec3> sampleRays;
    sampleRays.clear();

    vec3 sum(0);

    float phiOffset = PI / phiSteps * 0.5f;

    for (int phiStep = 0; phiStep < phiSteps; phiStep++)
    {
        float phi = -PI / 2.f + (PI * phiStep) / phiSteps + phiOffset;
        for (int thetaStep = 0; thetaStep < thetaSteps; thetaStep++)
        {
            float theta = (PI * 2.f * thetaStep) / thetaSteps;
            vec3 direction(cosf(theta), sinf(theta), sinf(phi));
            sum += normalize(direction);
            sampleRays.push_back(normalize(direction));
        }
    }
    cout << "SDF: Grids ["<< _grids.x << " " <<  _grids.y  << " " << _grids.z << "]"  << endl;
    int vectorSize = _grids.x * _grids.y * _grids.z;
    _voxels = vector<float>(vectorSize);
    _vertices = vector<vec3>( vectorSize );
    cout << "SDF: Calculate distance field voxels" << endl;

    auto start = std::chrono::system_clock::now();

    _progress = 0.f;
    _duration = 0.f;

    // for (int zIndex = 0; zIndex < dfDimensions.z; zIndex++)
    // {
    //     generate_layer(model,sampleRays, zIndex);
    //     cout << (int)trunc((zIndex + 1) * 100.f / dfDimensions.z) << "% complete ... " << endl;
    // }

    // Parallelization
    using namespace std::chrono_literals;

    std::vector<float>  _layer_progress(dfDimensions.z);

    /* Run some task on new thread. The launch policy std::launch::async
       makes sure that the task is run asynchronously on a new thread. */
    auto future = std::async(std::launch::async, [&] {
        std::vector<int> zid (dfDimensions.z);
        std::iota (std::begin(zid), std::end(zid), 0); // Fill with 0, 1, ..., 99.

        std::for_each(
        std::execution::par,
        zid.begin(),
        zid.end(),
        [&](auto&& zIndex)
        {
            cout << "\tProcessing layer: " << zIndex << endl;
            generate_layer(model,sampleRays, zIndex, _layer_progress[zIndex] );
        });
        return 0;
    });

    // Use wait_for() with zero milliseconds to check thread status.
    while (future.wait_for(0ms) != std::future_status::ready) {
        float progress_count = 0.f;
        for ( auto it : _layer_progress ) {
            progress_count += it;
        }

        _progress = progress_count / ( _layer_progress.size() );

        //cout << "SDF: Progress: " << _progress*100.f  << "%       \r";
        auto now = std::chrono::system_clock::now();
        _duration = std::chrono::duration_cast<std::chrono::milliseconds>(now - start).count() * 0.001; 

        std::this_thread::sleep_for(10ms);
    }
    auto result = future.get(); // Get result.
    _progress =1.f;

    //cout << "Path:" << model->_filename << endl;

    // Ouput time to file
    string OutPath;
    OutPath = model->_filename + ".time.txt";
    ofstream out_test;
    out_test.open(OutPath, ios::app | ios::out);
    if (!out_test.is_open()) {
        cout << "OutputERROR" << endl;
    }
    out_test << _voxels.size() << "," << _sample_rays << "," << _duration << endl;

    //cout << "Resolution:" << _resolution << " Time:" << _duration << "s" << endl;

    cout << "SDF: Calculation finished" << endl;
}

void SDF::generate_layer( const MODEL *model, vector<vec3> &sampleRays,int zIndex, float &progress)
{
    const ivec3 &dimensions = _grids;
    vec3 boxSize = _bounds.size();
    vec3 voxelSize(boxSize.x / dimensions.x,
                   boxSize.y / dimensions.y,
                   boxSize.z / dimensions.z);

    float maxLength = glm::length(boxSize);
    ray_t ray;
    intersection_t inter;

    float inv_total = 1.f /( dimensions.y* dimensions.x);
    int num = 0;

    for (int yIndex = 0; yIndex < dimensions.y; yIndex++)
    {
        for (int xIndex = 0; xIndex < dimensions.x; xIndex++)
        {
            // offset, refer to output buffer
            int index = zIndex * dimensions.y * dimensions.x +
                        yIndex * dimensions.x + xIndex;
            vec3 position = _bounds[0] + voxelSize * vec3(xIndex + 0.5f, yIndex + 0.5f, zIndex + 0.5f);

            ray.origin = position;

            // cout << glm::to_string(position) << " " ;

            int hit = 0;
            int hitBack = 0;
            float minDist = maxLength;
            for (vec3 &dir : sampleRays)
            {

                ray.direction = dir;

                inter.t = FLOAT_MAX;
                if (model->hit( ray ,inter))
                {
                    hit++;
                    if (dot(dir, inter.normal) > 0)
                    {
                        hitBack++;
                    }
                    if (inter.t < minDist)
                        minDist = inter.t  ; // shortest distance to model among rays
                }
            }
            if (hitBack > 0.5f * _sample_rays || hitBack > 0.9f * hit)
            {
                minDist = -minDist;
            }

            _voxels[index] = minDist;
            _vertices[index] = position;

            progress = float(++num) * inv_total;
            // cout << ((minDist > 0) ? 1 : 0) << " ";
        }
        // cout << endl;
    }

    progress = 1.f;
}


bool SDF::load( const std::string &source ) {
    std::string path = source + ".sdf";
    
    ifstream ifs;
    ifs.open( path);

    if ( !ifs.is_open() ) {
        std::cout << "SDF: Falid to load SDF texture:" << path << std::endl;
        return false;
    } 

    ifs >> (*this);
    ifs.close();
    std::cout << "SDF: Texture: " <<  path  << " loaded." << std::endl;

    return true;
} 

void SDF::save( const std::string &source)
{
    std::string path =  source + ".sdf";
    ofstream ofs;
    ofs.open(path);

    if (!ofs.is_open())
    {
        cout << "SDF:  Falid to save SDF texture." << endl;
        cout << path << endl;
    }
    else
    {
        this->_source = source;

        ofs << (*this);
        ofs.close();
        cout << "SDF: Texture: " <<  path  << " saved." << endl;
    }
}
