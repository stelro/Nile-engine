#pragma once

#include <glm/glm.hpp>

namespace nile {

  namespace Math {
    glm::mat4 lookAt( const glm::vec3 &position, const glm::vec3 &target,
                      const glm::vec3 &up ) noexcept;


  }    // namespace Math

}    // namespace nile
