#pragma once

#include "Nile/core/types.hh"
#include "Nile/utils/vertex.hh"

#include <vector>

namespace nile {

  class Texture2D;

  struct Mesh {

    Mesh( std::vector<Vertex> t_vertices, std::vector<u32> t_indices,
          std::vector<Texture2D *> t_textures ) noexcept
        : verticies( std::move( t_vertices ) )
        , indices( std::move( t_indices ) )
        , textures( std::move( t_textures ) ) {}

    u32 vao;
    u32 vbo;
    u32 ebo;

    std::vector<Vertex> verticies;
    std::vector<u32> indices;
    std::vector<Texture2D *> textures;
  };
}    // namespace nile
