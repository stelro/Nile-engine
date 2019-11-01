#pragma once

#include "Nile/asset/asset.hh"
#include "Nile/renderer/mesh.hh"

#include <vector>

// Model - each model represents an ( usually ) 3D object in the scene
// and each model may consist of several sub-models/shapes which we are refere to
// as Mesh or Meshes. So instead of load seperate each mesh to represent one signle
// model in the scene, we load a model as a whole
// ( with ModelBuilder via wavefront obj files )
// and then render it to the scene.

namespace nile {

  class Model : public Asset {
  public:
    Model( std::vector<Mesh> t_meshes ) noexcept
        : meshes( t_meshes ) {}
    std::vector<Mesh> meshes;
  };

}    // namespace nile
