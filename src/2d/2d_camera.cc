/* ================================================================================
$File: 2d_camera.cc
$Date: $
$Revision: $
$Creator: Rostislav Orestis Stelmach
$Notice: $
================================================================================ */

#include "Nile/2d/2d_camera.hh"
#include "Nile/core/settings.hh"

namespace nile {

  Camera2D::Camera2D( std::shared_ptr<Settings> settings ) noexcept
      : m_settings( settings ) {
    // m_projectionMatrix =
    //     glm::ortho( 0.0f, static_cast<f32>( m_settings->getWidth() ),
    //                 static_cast<f32>( m_settings->getHeight() ), 0.0f, -1.0f, 1.0f );
    m_projectionMatrix = glm::ortho( 0.0f, static_cast<f32>( m_settings->getWidth() ),
                                     static_cast<f32>( m_settings->getHeight() ), 0.0f );
    m_cameraMatrix = m_projectionMatrix;
  }

  Camera2D::~Camera2D() noexcept {}

  void Camera2D::update( [[maybe_unused]] float dt ) noexcept {

    if ( m_shouldCameraUpdate ) {
      glm::vec3 translation = glm::vec3( m_position, 1.0f );
      m_cameraMatrix = glm::translate( m_projectionMatrix, translation );
      glm::vec3 scale = glm::vec3( m_scale, 1.0f );
      m_cameraMatrix = glm::scale( m_cameraMatrix, scale );
      m_shouldCameraUpdate = false;
    }
  }

}    // namespace nile
