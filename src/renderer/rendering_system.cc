#include "Nile/renderer/rendering_system.hh"
#include "Nile/ecs/components/renderable.hh"
#include "Nile/ecs/components/transform.hh"
#include "Nile/ecs/ecs_coordinator.hh"

#include <glm/glm.hpp>

namespace nile {

  void RenderingSystem::init( const std::shared_ptr<Coordinator> &coordinator ) noexcept {
    m_ecsCoordinator = coordinator;
  }

  void RenderingSystem::update( float dt ) noexcept {
   
    for ( const auto &entity : m_entities ) {
      auto &tranform = m_ecsCoordinator->getComponent<Transform>( entity );
      auto &render = m_ecsCoordinator->getComponent<Renderable>( entity );

      tranform.position *= dt;
      render.color *= dt;
       log::print( "Hello from system render\n" );

    }
  }

}    // namespace nile
