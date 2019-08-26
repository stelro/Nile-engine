#pragma once

#include "core/nile.hh"
#include "renderer/shader.hh"
#include "renderer/texture2d.hh"

namespace nile {

  class SpriteRenderer {
  private:
    Shader m_shader;
    u32 m_quadVAO;

    void initRenderData() noexcept;

  public:
    SpriteRenderer( Shader &shader ) noexcept;
    ~SpriteRenderer() noexcept;

    void draw( Texture2D &texture, glm::vec2 position, glm::vec2 size = glm::vec2( 10, 10 ),
               f32 roate = 0.0f, glm::vec3 color = glm::vec3( 1.0f ) ) noexcept;

  };
}    // namespace nile
