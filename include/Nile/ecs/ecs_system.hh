/* ================================================================================
$File: ecs_system.hh
$Date: 09 Oct 2019 $
$Revision: $
$Creator: Rostislav Orestis Stelmach
$Notice: $
================================================================================ */

#pragma once

#include "Nile/core/types.hh"
#include <set>

namespace nile {

  class System {
  public:
    std::set<Entity> entities_;
  };

}    // namespace nile
