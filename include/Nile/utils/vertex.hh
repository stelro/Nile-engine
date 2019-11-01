#pragma once

#include <glm/glm.hpp>

namespace nile {

  struct Vertex {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 uv;
  };

}    // namespace nile
