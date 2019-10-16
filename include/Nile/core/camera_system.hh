#pragma once


#include "Nile/ecs/ecs_system.hh"
#include <glm/glm.hpp>
#include <memory>

namespace nile {

  class Coordinator;
  class Settings;

  class CameraSystem : public System {
  private:
    std::shared_ptr<Coordinator> m_ecsCoordinator;
    std::shared_ptr<Settings> m_settings;

    // its static for now, because it doesn't affect any members of the class
    // but should be moved sometime to math utility class
    static glm::mat4 lookAt( const glm::vec3 &position, const glm::vec3 &target,
                             const glm::vec3 &up ) noexcept;

  public:
    CameraSystem( const std::shared_ptr<Coordinator> &coordinator,
                  const std::shared_ptr<Settings> m_settings ) noexcept;
    void create() noexcept;
    void update( f32 dt ) noexcept;
    void destroy() noexcept;
  };

}    // namespace nile
