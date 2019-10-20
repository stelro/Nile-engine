#include "Nile/math/utils.hh"

namespace nile::Math {

  glm::mat4 lookAt( const glm::vec3 &position, const glm::vec3 &target,
                    const glm::vec3 &up ) noexcept {

    glm::vec3 forward = glm::normalize( position - target );

    glm::vec3 right = glm::normalize( glm::cross( glm::normalize( up ), forward ) );

    glm::vec3 updir = glm::cross( forward, right );

    glm::mat4 rotation {1.0f};
    glm::mat4 translation {1.0f};

    rotation[ 0 ][ 0 ] = right.x;
    rotation[ 1 ][ 0 ] = right.y;
    rotation[ 2 ][ 0 ] = right.z;
    rotation[ 0 ][ 1 ] = updir.x;
    rotation[ 1 ][ 1 ] = updir.y;
    rotation[ 2 ][ 1 ] = updir.z;
    rotation[ 0 ][ 2 ] = forward.x;
    rotation[ 1 ][ 2 ] = forward.y;
    rotation[ 2 ][ 2 ] = forward.z;

    translation[ 3 ][ 0 ] = -position.x;
    translation[ 3 ][ 1 ] = -position.y;
    translation[ 3 ][ 2 ] = -position.z;

    return rotation * translation;
  }


}    // namespace nile::Math
