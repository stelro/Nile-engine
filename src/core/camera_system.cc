#include "Nile/core/camera_system.hh"
#include "Nile/core/settings.hh"
#include "Nile/ecs/components/camera_component.hh"
#include "Nile/ecs/components/transform.hh"
#include "Nile/ecs/ecs_coordinator.hh"
#include <glm/gtc/matrix_transform.hpp>

namespace nile {

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
          cameraComponent.projectionMatrix =
              glm::perspective( glm::radians( cameraComponent.fieldOfView ),
                                static_cast<f32>( m_settings->getWidth() ) /
                                    static_cast<f32>( m_settings->getHeight() ),
                                cameraComponent.near, cameraComponent.far );
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
        glm::vec3 translation = transform.position;
        cameraComponent.cameraMatrix =
            glm::translate( cameraComponent.projectionMatrix, translation );
        cameraComponent.cameraMatrix = glm::scale( cameraComponent.cameraMatrix, transform.scale );

        // compute the view matrix
        glm::vec3 front;
        front.x = cos( glm::radians( cameraComponent.yaw ) ) *
                  cos( glm::radians( cameraComponent.pitch ) );
        front.y = sin( glm::radians( cameraComponent.pitch ) );
        front.z = sin( glm::radians( cameraComponent.yaw ) ) *
                  cos( glm::radians( cameraComponent.pitch ) );
        cameraComponent.cameraFront = glm::normalize( front );
        cameraComponent.cameraRight =
            glm::normalize( glm::cross( cameraComponent.cameraFront, cameraComponent.cameraUp ) );
        cameraComponent.up = glm::normalize(
            glm::cross( cameraComponent.cameraRight, cameraComponent.cameraFront ) );
        cameraComponent.shouldCameraUpdate = false;

      }
    }
  }

  void CameraSystem::destroy() noexcept {}

}    // namespace nile
