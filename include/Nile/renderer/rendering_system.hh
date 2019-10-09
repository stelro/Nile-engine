/* ================================================================================
$File: rendering_system.hh
$Date: 09 Oct 2019 $
$Revision: $
$Creator: Rostislav Orestis Stelmach
$Notice: $
================================================================================ */

#pragma once 

#include "Nile/ecs/ecs_system.hh"

#include <memory>

namespace nile {
  
  class Coordinator;

  class RenderingSystem : public System {
    private:
      std::shared_ptr<Coordinator> m_ecsCoordinator;
    public:
      void init(const std::shared_ptr<Coordinator>& coordinator) noexcept;
      void update(float dt) noexcept;
  };
}
