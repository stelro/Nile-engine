/* ================================================================================
$File: transform_system.hh
$Date: 19 Jul 2020 $
$Revision: $
$Creator: Rostislav Orestis Stelmach
$Notice: $
================================================================================ */

#pragma once 

#include "Nile/core/types.hh"
#include "Nile/ecs/ecs_system.hh"

#include <memory>

namespace nile {

  class Coordinator;
  
  class TransformSystem : public System {
  public:
    TransformSystem(const std::shared_ptr<Coordinator>& coordinator);

    void create();
    void update();
    void render();
  private:
    std::shared_ptr<Coordinator> ecs_coordinator_;

  };

}


