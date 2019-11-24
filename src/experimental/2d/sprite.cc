/* ================================================================================
$File: sprite.cc
$Date: 03 Oct 2019
$Revision: $
$Creator: Rostislav Orestis Stelmach
$Notice: $
================================================================================ */

#include "Nile/2d/sprite.hh"
#include "Nile/renderer/texture2d.hh"

namespace nile {

  Sprite::Sprite( const std::shared_ptr<Texture2D> &texture ) noexcept
      : m_texture( texture ) {}


  void Sprite::setPosition( const glm::vec2 &position ) noexcept {
    m_position = position;
  }

  void Sprite::setColor( const glm::vec3 &color ) noexcept {
    m_color = color;
  }

  void Sprite::setSize( const glm::vec2 &size ) noexcept {
    m_size = size;
  }

  void Sprite::setRotation( f32 rotation ) noexcept {
    m_rotation = rotation;
  }

  glm::vec2 Sprite::getPosition() const noexcept {
    return m_position;
  }

  glm::vec2 Sprite::getSize() const noexcept {
    return m_size;
  }

  glm::vec3 Sprite::getColor() const noexcept {
    return m_color;
  }

  f32 Sprite::getRotation() const noexcept {
    return m_rotation;
  }

  std::shared_ptr<Texture2D> Sprite::getTexture() const noexcept {
    return m_texture;
  }

}    // namespace nile

