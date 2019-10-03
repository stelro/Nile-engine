/* ================================================================================
$File: sprite_sheet.hh
$Date: $
$Revision: $
$Creator: Rostislav Orestis Stelmach
$Notice: $
================================================================================ */

#pragma once

#include "Nile/core/nile.hh"
#include "Nile/core/signal.hh"

#include <glm/glm.hpp>

namespace nile {

  class ShaderSet;
  class Texture2D;

  // NW         NE
  //      N
  // W ---|---  E
  //      S
  // SW         SE

  enum class TextureOrientation
  {
    NE,    // North East
    SE,    // South East
    SW,    // South West
    NW,    // North West
  };

  class SpriteSheet {
    OBSERVABLE
  private:
    ShaderSet *m_shader;
    Texture2D *m_texture;

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

    TextureOrientation m_textureOrientation;

    // First Initializations happens here
    // TODO(stel): maybe we should to fix this?!
    void initRenderData() noexcept;

  public:
    using Animation_signal = Signal<bool>;

    // This signal should be emitted when the method "playAnimationAndHalt" will play
    // the whole animation, and the frame_count will reach the last frame. Then the sinal
    // will emit(true), so input handlers will know that the input events can continue
    // for example, if player want to attack, then specific animation 'attack' will take
    // place, and we don't to interput this animation with other input, until it will end.
    //
    // TODO(stel): maybe there is a better way to handle this?
    Animation_signal animation_signal;

    SpriteSheet( ShaderSet *shader, Texture2D *texture, const glm::ivec2 &dimensions ) noexcept;
    ~SpriteSheet() noexcept;

    void draw_frame( const glm::vec2 &position, u32 frame_index, u32 speed = 60 ) noexcept;
    void playAnimation( const glm::vec2 &position, u32 speed = 60 ) noexcept;
    void playAnimationAndHalt( const glm::vec2 &position, u32 speed = 60 ) noexcept;

    // Setters
    void setColor( const glm::vec3 &color ) noexcept;
    void setSize( const glm::vec2 &size ) noexcept;
    void setRotationAngle( f32 angle ) noexcept;
    void scale( f32 scalar ) noexcept;

    // Flip the texture horizontally considering the current location it's looking
    void setTextureOrientation( TextureOrientation orientation ) noexcept;
  };
}    // namespace nile
