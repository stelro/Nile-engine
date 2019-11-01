#pragma once

#include "Nile/asset/builder/asset_builder.hh"
#include "Nile/core/types.hh"
#include "Nile/renderer/mesh.hh"
#include "Nile/renderer/model.hh"
#include "Nile/renderer/texture2d.hh"
#include "Nile/asset/asset_manager.hh"

#include "assimp/Importer.hpp"
#include "assimp/postprocess.h"
#include "assimp/scene.h"

#include <memory>
#include <string_view>
#include <vector>

namespace nile::AssetBuilder {

  class AssetManager;

  template <>
  class Builder<Model> final {
  private:
    std::string m_directoryName;
    std::string m_path;

    std::vector<Mesh> m_meshes;
    std::shared_ptr<nile::AssetManager> m_assetManager;

    void loadModel() noexcept;
    void processNode( aiNode *node, const aiScene *scene ) noexcept;
    Mesh processMesh( aiMesh *mesh, const aiScene *scene ) noexcept;

    std::vector<Texture2D*> loadMaterialTextures( aiMaterial *material,
                                                 aiTextureType type ) noexcept;

  public:
    Builder( const std::shared_ptr<nile::AssetManager> &assetManager ) noexcept;
    Builder( Builder && ) = default;
    Builder( const Builder & ) = default;
    Builder &operator=( Builder && ) = default;
    Builder &operator=( const Builder & ) = default;

    Builder &setModelPath( std::string_view path ) noexcept;

    [[nodiscard]] Model *build() noexcept;
  };

}    // namespace nile::AssetBuilder
