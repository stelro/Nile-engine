#pragma once

#include "Nile/core/types.hh"
#include <glm/glm.hpp>

namespace nile {

  struct Primitive {
    // line begin x/y
    glm::vec2 begin {0.0f};
    // line end x/y
    glm::vec2 end {1.0f};
    // line width
    f32 lineWidth {1.0f};
    u32 vao;
    // vertex buffer object
    u32 vbo;
  };

}    // namespace nile
