#pragma once 

#include "Nile/utils/vertex.hh"
#include "Nile/core/types.hh"

#include <vector>

namespace nile {

  class Texture2D;

  struct MeshComponent {
    std::vector<Vertex> vertices;
    std::vector<u32> indices;
    std::vector<Texture2D*> textures;
    u32 vbo;
    u32 vao;
    u32 ebo;
  };

}
