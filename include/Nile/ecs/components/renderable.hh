/* ================================================================================
$File: renderable.hh
$Date: 09 Oct 2019 $
$Revision: $
$Creator: Rostislav Orestis Stelmach
$Notice: $
================================================================================ */

#pragma once

#include <glm/glm.hpp>

namespace nile {

  struct Renderable {
    Renderable() = default;

    Renderable( const glm::vec3 &c )
        : color( c ) {}

    glm::vec3 color {};
  };

}    // namespace nile
