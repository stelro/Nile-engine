#include "Nile/core/camera_system.hh"
#include "Nile/core/settings.hh"
#include "Nile/ecs/components/camera_component.hh"
#include "Nile/ecs/components/transform.hh"
#include "Nile/ecs/ecs_coordinator.hh"
#include <glm/gtc/matrix_transform.hpp>

namespace nile {

  glm::mat4 CameraSystem::lookAt( const glm::vec3 &position, const glm::vec3 &target,
                                  const glm::vec3 &up ) noexcept {

    glm::vec3 forward = glm::normalize( position - target );

    glm::vec3 right = glm::normalize( glm::cross( glm::normalize( up ), forward ) );

    glm::vec3 updir = glm::cross( forward, right );

    glm::mat4 rotation {1.0f};
    glm::mat4 translation {1.0f};

    rotation[ 0 ][ 0 ] = right.x;
    rotation[ 1 ][ 0 ] = right.y;
    rotation[ 2 ][ 0 ] = right.z;
    rotation[ 0 ][ 1 ] = updir.x;
    rotation[ 1 ][ 1 ] = updir.y;
    rotation[ 2 ][ 1 ] = updir.z;
    rotation[ 0 ][ 2 ] = forward.x;
    rotation[ 1 ][ 2 ] = forward.y;
    rotation[ 2 ][ 2 ] = forward.z;

    translation[ 3 ][ 0 ] = -position.x;
    translation[ 3 ][ 1 ] = -position.y;
    translation[ 3 ][ 2 ] = -position.z;

    return rotation * translation;
  }

  CameraSystem::CameraSystem( const std::shared_ptr<Coordinator> &coordinator,
                              const std::shared_ptr<Settings> settings ) noexcept
      : m_ecsCoordinator( coordinator )
      , m_settings( settings ) {}

  void CameraSystem::create() noexcept {

    // Setup the camera, create the projection matrix
    for ( const auto &entity : m_entities ) {

      auto &cameraComponent = m_ecsCoordinator->getComponent<CameraComponent>( entity );

      switch ( cameraComponent.projectionType ) {
        case ProjectionType::ORTHOGRAPHIC:
          cameraComponent.projectionMatrix =
              glm::ortho( 0.0f, static_cast<f32>( m_settings->getWidth() ),
                          static_cast<f32>( m_settings->getHeight() ), 0.0f );
          break;
        case ProjectionType::PERSPECTIVE:
          // TODO(stel): create perspective projection matrix
          break;
        default:
          break;
      };
    }
  }

  void CameraSystem::update( f32 dt ) noexcept {

    for ( const auto &entity : m_entities ) {
      auto &transform = m_ecsCoordinator->getComponent<Transform>( entity );
      auto &cameraComponent = m_ecsCoordinator->getComponent<CameraComponent>( entity );
      if ( cameraComponent.shouldCameraUpdate ) {
        log::print( "should updtate\n" );

        glm::vec3 translation = transform.position;
        cameraComponent.cameraMatrix =
            glm::translate( cameraComponent.projectionMatrix, translation );
        cameraComponent.cameraMatrix = glm::scale( cameraComponent.cameraMatrix, transform.scale );

        // compute the view matrix
        cameraComponent.viewMatrix =
            lookAt( transform.position, transform.position + cameraComponent.cameraFront,
                    cameraComponent.cameraUp );
        cameraComponent.shouldCameraUpdate = false;
      }
    }
  }

  void CameraSystem::destroy() noexcept {}

}    // namespace nile
