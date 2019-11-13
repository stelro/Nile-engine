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
        , xRotation( r )
        , yRotation( r )
        , zRotation( r ) {}

    Transform( const glm::vec3 &p, const glm::vec3 &s ) noexcept
        : position( p )
        , scale( s )
        , xRotation( 0.0f )
        , yRotation( 0.0f )
        , zRotation( 0.0f ) {}

    Transform( const glm::vec3 &p, const glm::vec3 &s, f32 x, f32 y, f32 z ) noexcept
        : position( p )
        , scale( s )
        , xRotation( x )
        , yRotation( y )
        , zRotation( z ) {}

    glm::vec3 position {1.0f};
    glm::vec3 scale {1.0f};
    f32 xRotation {};
    f32 yRotation {};
    f32 zRotation {};
  };

}    // namespace nile
