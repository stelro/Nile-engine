/* ================================================================================
$File: transform.hh
$Date: 09 Oct 2019 $
$Revision: $
$Creator: Rostislav Orestis Stelmach
$Notice: $
================================================================================ */

#pragma once

#include "Nile/core/types.hh"
#include <glm/glm.hpp>

namespace nile {

  struct Transform {

    Transform() = default;
    Transform( const glm::vec3 &p, const glm::vec3 &s, f32 r ) noexcept
        : position( p )
        , scale( s )
        , rotation( r ) {}

    glm::vec3 position {1.0f};
    glm::vec3 scale {1.0f};
    f32 rotation {};
  };

}    // namespace nile
