#pragma once

#include "Nile/core/types.hh"
#include "Nile/experimental/texture.hh"
#include "Nile/experimental/vertex.hh"
#include "Nile/renderer/shaderset.hh"


#include <vector>

namespace nile::experimental {


  class Mesh {
  private:
    u32 m_vao, m_vbo, m_ebo;
    void setupMesh() noexcept;

  public:
    std::vector<Vertex> vertices;
    std::vector<u32> indices;
    std::vector<Texture> textures;


    Mesh( std::vector<Vertex> vertices, std::vector<u32> indices,
          std::vector<Texture> textures ) noexcept;

    void draw( ShaderSet *shader );
  };

}    // namespace nile::experimental
