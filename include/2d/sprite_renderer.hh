#pragma once

#include "core/nile.hh"
#include "renderer/texture2d.hh"
#include <glm/glm.hpp>

#include <memory>

namespace nile {

  class Shader;

  class SpriteRenderer {
  private:
    std::shared_ptr<Shader> m_shader;
    u32 m_quadVAO;

    void initRenderData() noexcept;

  public:
    SpriteRenderer( std::shared_ptr<Shader> shader ) noexcept;
    ~SpriteRenderer() noexcept;

    void draw( std::shared_ptr<Texture2D> texture, glm::vec2 position,
               glm::vec2 size = glm::vec2( 10, 10 ), f32 roate = 0.0f,
               glm::vec3 color = glm::vec3( 1.0f ) ) noexcept;
  };
}    // namespace nile
