#pragma once

#include <glm/glm.hpp>

namespace nile {

  namespace experimental {

    struct Vertex {
      glm::vec3 position;
      glm::vec3 normal;
      glm::vec2 texCoords;
    };

  }    // namespace experimental

}    // namespace nile
