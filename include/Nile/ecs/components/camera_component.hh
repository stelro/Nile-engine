#pragma once

#include "Nile/core/types.hh"
#include <glm/glm.hpp>

namespace nile {

  enum class ProjectionType { ORTHOGRAPHIC, PERSPECTIVE };

  struct CameraComponent {
    CameraComponent( f32 n, f32 f, f32 fov, ProjectionType type ) noexcept
        : near( n )
        , far( f )
        , fieldOfView( fov )
        , projectionType( type ) {}

    CameraComponent() noexcept
        : near( 1.0f )
        , far( 100.f )
        , fieldOfView( 45.0f )
        , projectionType( ProjectionType::ORTHOGRAPHIC ) {}

    f32 near;
    f32 far;
    f32 fieldOfView;
    glm::vec3 cameraFront {0.0f, 0.0f, -1.0f};
    glm::vec3 cameraUp { 0.0f, 1.0f, 0.0f};
    glm::mat4 projectionMatrix {1.0f};
    glm::mat4 viewMatrix {1.0f};
    glm::mat4 cameraMatrix {1.0f};
    ProjectionType projectionType;
    // Flag to check whenever we should update the camera
    // this should be set to true only when some of the camera component or
    // transform values
    // has changed
    bool shouldCameraUpdate = true;
  };

}    // namespace nile
