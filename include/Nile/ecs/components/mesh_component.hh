#pragma once

#include "Nile/core/types.hh"
#include "Nile/utils/vertex.hh"

#include <memory>
#include <vector>

namespace nile {

  class Texture2D;

  struct MeshComponent {
    std::vector<Vertex> vertices;
    std::vector<u32> indices;
    std::vector<std::shared_ptr<Texture2D>> textures;
    u32 vbo;
    u32 vao;
    u32 ebo;
  };

}    // namespace nile
