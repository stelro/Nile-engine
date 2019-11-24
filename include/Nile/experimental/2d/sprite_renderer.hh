/* ================================================================================
$File: sprite_renderer.hh
$Date: $
$Revision: $
$Creator: Rostislav Orestis Stelmach
$Notice: $
================================================================================ */

#pragma once

#include "Nile/2d/sprite.hh"
#include "Nile/core/types.hh"
#include "Nile/renderer/texture2d.hh"
#include <glm/glm.hpp>

#include <memory>

namespace nile {

  class ShaderSet;

  class SpriteRenderer {
  private:
    std::shared_ptr<ShaderSet> m_shader;
    u32 m_quadVAO;

    void initRenderData() noexcept;

  public:
    SpriteRenderer( const std::shared_ptr<ShaderSet> &shader ) noexcept;
    ~SpriteRenderer() noexcept;

    void draw( const std::shared_ptr<Texture2D> &texture, glm::vec2 position,
               glm::vec2 size = glm::vec2( 10, 10 ), f32 roate = 0.0f,
               glm::vec3 color = glm::vec3( 1.0f ) ) noexcept;

    void draw( const Sprite &sprite ) noexcept;
  };
}    // namespace nile
