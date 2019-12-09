/* ================================================================================
$File: renderable.hh
$Date: 09 Oct 2019 $
$Revision: $
$Creator: Rostislav Orestis Stelmach
$Notice: $
================================================================================ */

#pragma once

#include <memory>

#include <glm/glm.hpp>

namespace nile {

  class ShaderSet;

  struct Renderable {
    Renderable() = default;

    Renderable( const glm::vec3 &c )
        : color( c ) {}

    std::shared_ptr<ShaderSet> shaderSet = nullptr;
    glm::vec3 color {1.0f, 1.0f, 1.0f};
    bool blend = true;
  };

}    // namespace nile
