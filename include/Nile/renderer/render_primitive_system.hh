#pragma once

#include "Nile/core/types.hh"
#include "Nile/ecs/ecs_system.hh"

#include <memory>

namespace nile {

  class Coordinator;
  class ShaderSet;

  class RenderPrimitiveSystem : public System {
  private:
    void init_rendering_data() noexcept;
    std::shared_ptr<Coordinator> ecs_coordinator_;
    std::shared_ptr<ShaderSet> primitive_shader_;

  public:
    RenderPrimitiveSystem( const std::shared_ptr<Coordinator> &coordinator,
                           const std::shared_ptr<ShaderSet> &shader ) noexcept;
    void create() noexcept;
    void destroy() noexcept;
    void render( float dt ) noexcept;
  };

}    // namespace nile
