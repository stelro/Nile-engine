#pragma once

#include "Nile/asset/asset_manager.hh"
#include "Nile/asset/builder/asset_builder.hh"
#include "Nile/core/types.hh"
#include "Nile/renderer/mesh.hh"
#include "Nile/renderer/model.hh"
#include "Nile/renderer/texture2d.hh"

#include "assimp/Importer.hpp"
#include "assimp/postprocess.h"
#include "assimp/scene.h"

#include <memory>
#include <string_view>
#include <vector>

namespace nile::AssetBuilder {

  enum class PostProcessFlags {
    // Calculates tanget and bitangets for the imported meshes
    calcTangetSpace = aiProcess_CalcTangentSpace,
    // identifies and joins identical vertex data
    joinIdenicalVertices = aiProcess_JoinIdenticalVertices,
    // coverts all the imported data to a left-handed coordinate-space
    makeLeftHanded = aiProcess_MakeLeftHanded,
    // triangulates all faces of the mesh
    triangulate = aiProcess_Triangulate,
    // Removes some parts of the data structure ( animations, materials,
    // light sources, cameras, textures, vertex compoentnt)
    removeComponent = aiProcess_RemoveComponent,
    // Generate normals for all the faces of all meshes
    genNormals = aiProcess_GenNormals,
    // Generate smooth normals
    genSmootNormals = aiProcess_GenSmoothNormals,
    // split large meshes into smaller sub-meshes
    spilLargeMeshes = aiProcess_SplitLargeMeshes,

  };

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

    std::vector<std::shared_ptr<Texture2D>> loadMaterialTextures( aiMaterial *material,
                                                                  aiTextureType type ) noexcept;

  public:
    Builder( const std::shared_ptr<nile::AssetManager> &assetManager ) noexcept;
    Builder( Builder && ) = default;
    Builder( const Builder & ) = default;
    Builder &operator=( Builder && ) = default;
    Builder &operator=( const Builder & ) = default;

    Builder &setModelPath( std::string_view path ) noexcept;

    [[nodiscard]] std::shared_ptr<Model> build() noexcept;
  };

}    // namespace nile::AssetBuilder
