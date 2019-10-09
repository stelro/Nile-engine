/* ================================================================================
$File: sprite.hh
$Date: 03 Oct 2019
$Revision: $
$Creator: Rostislav Orestis Stelmach
$Notice: $
================================================================================ */

#pragma once

#include "Nile/core/types.hh"
#include <glm/glm.hpp>

//@brief:
// Sprite is just a wrapper around texture 2D, which keeps more information
// on it, like textures position on the wold, its size, color and more
//
// * texture should be loaded form assetmanager

namespace nile {
 
  class Texture2D;
  class Sprite {
  private:
    Texture2D *m_texture;
    glm::vec2 m_position;
    glm::vec2 m_size;
    glm::vec3 m_color;
    f32 m_rotation;

  public:
    Sprite( Texture2D *texture ) noexcept;
    Sprite(const Sprite& rhs) noexcept;
    Sprite(Sprite&& rhs) noexcept;
    Sprite& operator =(const Sprite& rhs) noexcept;
    Sprite& operator =(Sprite&& rhs) noexcept;
    ~Sprite() noexcept;

    void setPosition( const glm::vec2 &position ) noexcept;
    void setColor( const glm::vec3 &color ) noexcept;
    void setSize( const glm::vec2 &size ) noexcept;
    void setRotation( f32 rotation ) noexcept;

    glm::vec2 getPosition() const noexcept;
    glm::vec2 getSize() const noexcept;
    glm::vec3 getColor() const noexcept;
    f32 getRotation() const noexcept;
    Texture2D* getTexture() const noexcept;
  };

}    // namespace nile

