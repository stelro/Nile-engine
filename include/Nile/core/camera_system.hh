#pragma once


#include "Nile/ecs/ecs_system.hh"
#include <glm/glm.hpp>
#include <memory>

namespace nile {

  class Coordinator;
  class Settings;

  class CameraSystem : public System {
  private:
    std::shared_ptr<Coordinator> ecs_coordinator_;
    std::shared_ptr<Settings> settings_;

  public:
    CameraSystem( const std::shared_ptr<Coordinator> &coordinator,
                  const std::shared_ptr<Settings>& settings ) noexcept;
    void create() noexcept;
    void update( f32 dt ) noexcept;
    void destroy() noexcept;
  };

}    // namespace nile
