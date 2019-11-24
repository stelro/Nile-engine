/* ================================================================================
$File: sprite_rendering_system.hh
$Date: 09 Oct 2019 $
$Revision: $
$Creator: Rostislav Orestis Stelmach
$Notice: $
================================================================================ */
#pragma once

#include "Nile/ecs/ecs_system.hh"

#include <memory>

// @brief:
// SpriteRenderingSystem basiclly render/draws sprites to the screen
// an sprite is :
// transformable
// renderable
// and has sprite ( texture ) //TODO(stel): maybe get better name for this component?

namespace nile {

  class ShaderSet;
  class Coordinator;

  class SpriteRenderingSystem : public System {
  private:
    void initRenderData() noexcept;
    std::shared_ptr<Coordinator> m_ecsCoordinator;
    std::shared_ptr<ShaderSet> m_spriteShader;
    u32 m_quadVAO;

  public:
    SpriteRenderingSystem( const std::shared_ptr<Coordinator> &coordinator,
                           const std::shared_ptr<ShaderSet> &shader ) noexcept;
    void create() noexcept;
    void destroy() noexcept;
    void update( float dt ) noexcept;
    void render( float dt ) noexcept;
  };

}    // namespace nile
