#pragma once

#include "core/nile.hh"
#include <glm/glm.hpp>

#include <memory>

namespace nile {

  class Shader;
  class Texture2D;

  class SpriteSheet {
  private:
    std::shared_ptr<Shader> m_shader;
    std::shared_ptr<Texture2D> m_texture;

    // This is the width and height of the cropped sprite
    // e.g ( 64 x 64 ), and not from the actual texture
    glm::ivec2 m_spriteDimensions;

    glm::vec3 m_color;
    glm::vec2 m_size;
    f32 m_rotateAngle;

    u32 m_numberOfColumns;
    u32 m_numberOfRows;

    // Quad vertex attribute object
    u32 m_quadVAO;
    // Quad Vertex Buffer Object, used for vertices and texture coordinates
    u32 m_quadVBO;

    int m_animationDelay = 0;
    u32 m_currentFrame = 0;

    // First Initializations happens here
    // TODO(stel): maybe we should to fix this?!
    void initRenderData() noexcept;

  public:
    SpriteSheet( const std::shared_ptr<Shader> &shader, const std::shared_ptr<Texture2D> &texture,
                 const glm::ivec2 &dimensions ) noexcept;
    ~SpriteSheet() noexcept;

    void draw_frame( const glm::vec2 &position, u32 frame_index, u32 speed = 60 ) noexcept;
    void playAnimation( const glm::vec2 &position, u32 speed = 60 ) noexcept;

    // Setters
    void setColor( const glm::vec3 &color ) noexcept;
    void setSize( const glm::vec2 &size ) noexcept;
    void setRotationAngle( f32 angle ) noexcept;
    void scale( f32 scalar ) noexcept;
  };
}    // namespace nile
