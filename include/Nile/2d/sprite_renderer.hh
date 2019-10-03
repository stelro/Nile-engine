/* ================================================================================
$File: sprite_renderer.hh
$Date: $
$Revision: $
$Creator: Rostislav Orestis Stelmach
$Notice: $
================================================================================ */

#pragma once

#include "Nile/core/nile.hh"
#include "Nile/renderer/texture2d.hh"
#include <glm/glm.hpp>


namespace nile {

  class ShaderSet;

  class SpriteRenderer {
  private:
    ShaderSet* m_shader;
    u32 m_quadVAO;

    void initRenderData() noexcept;

  public:
    SpriteRenderer( ShaderSet* shader ) noexcept;
    ~SpriteRenderer() noexcept;

    void draw( Texture2D* texture, glm::vec2 position,
               glm::vec2 size = glm::vec2( 10, 10 ), f32 roate = 0.0f,
               glm::vec3 color = glm::vec3( 1.0f ) ) noexcept;
  };
}    // namespace nile
