#pragma once

#include "Nile/asset/asset_manager.hh"
#include "Nile/core/types.hh"
#include "Nile/experimental/mesh.hh"
#include "Nile/renderer/shaderset.hh"

#include "assimp/Importer.hpp"
#include "assimp/postprocess.h"
#include "assimp/scene.h"

#include <memory>
#include <vector>

namespace nile::experimental {

  class Model {
  private:
    std::vector<Mesh> meshes;
    std::string directory;
    std::shared_ptr<AssetManager> m_assetManager;

    void loadModel( std::string path );
    void processNode( aiNode *node, const aiScene *scene );
    Mesh processMesh( aiMesh *mesh, const aiScene *scene );

    std::vector<Texture> loadMaterialTextures( aiMaterial *mat, aiTextureType type,
                                               std::string typeName );

  public:
    Model( std::string path, const std::shared_ptr<AssetManager> &assetManager )
        : m_assetManager( assetManager ) {
      loadModel( path );
    }

    void draw( ShaderSet *shader );
  };

}    // namespace nile::experimental
