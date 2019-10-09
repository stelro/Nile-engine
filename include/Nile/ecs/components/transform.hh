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
    glm::vec3 position;
    glm::vec3 scale;
    f32 rotation;
  };

}    // namespace nile
