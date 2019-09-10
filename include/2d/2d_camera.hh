#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <memory>

// Basic 2D orthographic projection camera
// *
// When we're talking about camera/view space, we're talking about all the
// vertex coordinates as seen from the camera's perspective as the origin of the
// scene: The view matrix transforms all the world coordinates inte view coordinates
// that are relative to the camera position and direction.

namespace nile {

  class Settings;

  class Camera2D {
  private:
    glm::mat4 m_projectionMatrix;
    glm::mat4 m_cameraMatrix = glm::mat4(1.0f);
    glm::vec2 m_position = glm::vec2(1.0f);
    glm::vec2 m_scale = glm::vec2(1.0f);
    std::shared_ptr<Settings> m_settings;

    bool m_shouldCameraUpdate = false;
  public:
    Camera2D( std::shared_ptr<Settings> settings ) noexcept;
    ~Camera2D() noexcept;

    // Setters
    void setPosition( const glm::vec2 &position ) noexcept {
      m_position = position;
      m_shouldCameraUpdate = true;
    }

    void setScale( const glm::vec2 &scale ) noexcept {
      m_scale = scale;
      m_shouldCameraUpdate = true;
    }

    // Getters

    [[nodiscard]] glm::vec2 getPosition() const noexcept {
      return m_position;
    }

    [[nodiscard]] glm::vec2 getScale() const noexcept {
      return m_scale;
    }

    [[nodiscard]] glm::mat4 getCameraMatrix() const noexcept {
      return m_cameraMatrix;
    }

    [[nodiscard]] inline glm::mat4 getProjectionMatrix() const noexcept {
      return m_projectionMatrix;
    }

    void update( [[maybe_unused]] float dt ) noexcept;
  };
}    // namespace nile
