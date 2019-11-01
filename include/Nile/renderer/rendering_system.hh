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
    std::shared_ptr<Coordinator> m_ecsCoordinator;
    u32 m_vao;
    ShaderSet *m_shader;
    void initRenderData() noexcept;


  public:
    RenderingSystem( const std::shared_ptr<Coordinator> &coordinator, ShaderSet *shader ) noexcept;
    void create() noexcept;
    void update( float dt ) noexcept;
    void render( float dt ) noexcept;
  };
}    // namespace nile
