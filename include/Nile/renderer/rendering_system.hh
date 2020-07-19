/* ================================================================================
$File: rendering_system.hh
$Date: 09 Oct 2019 $
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
  class ShaderSet;

  class RenderingSystem : public System {
  private:
    std::shared_ptr<Coordinator> ecs_coordinator_;
    std::shared_ptr<ShaderSet> shader_;
    void init_rendering_data() noexcept;

  public:
    RenderingSystem( const std::shared_ptr<Coordinator> &coordinator,
                     const std::shared_ptr<ShaderSet> &shader ) noexcept;
    void create() noexcept;
    void update( float dt ) noexcept;
    void render( float dt ) noexcept;
  };
}    // namespace nile
