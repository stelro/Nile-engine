#pragma once

#include "Nile/core/types.hh"
#include "Nile/ecs/ecs_system.hh"

#include <memory>

namespace nile {

  class Coordinator;
  class ShaderSet;

  class RenderPrimitiveSystem : public System {
  private:
    void initRenderData() noexcept;
    std::shared_ptr<Coordinator> m_ecsCoordinator;
    std::shared_ptr<ShaderSet> m_primShader;

  public:
    RenderPrimitiveSystem( const std::shared_ptr<Coordinator> &coordinator,
                           const std::shared_ptr<ShaderSet> &shader ) noexcept;
    void create() noexcept;
    void destroy() noexcept;
    void render( float dt ) noexcept;
  };

}    // namespace nile
