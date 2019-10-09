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

  Sprite::Sprite( Texture2D *texture ) noexcept
      : m_texture( texture ) {
    m_texture->inc();
  }

  Sprite::~Sprite() noexcept {
    if ( m_texture ) {
      m_texture->dec();
    }
  }

  Sprite::Sprite( const Sprite &rhs ) noexcept
      : m_position( rhs.m_position )
      , m_size( rhs.m_size )
      , m_color( rhs.m_color )
      , m_rotation( rhs.m_rotation ) {
    m_texture = rhs.m_texture;
    m_texture->inc();
  }

  Sprite::Sprite( Sprite &&rhs ) noexcept
      : m_texture( rhs.m_texture )
      , m_position( rhs.m_position )
      , m_size( rhs.m_size )
      , m_color( rhs.m_color )
      , m_rotation( rhs.m_rotation ) {
    rhs.m_texture = nullptr;
  }

  Sprite &Sprite::operator=( const Sprite &rhs ) noexcept {

    if ( this != &rhs ) {
      m_texture->dec();
      m_texture = nullptr;
      m_texture = rhs.m_texture;
      m_texture->inc();
      *m_texture = *rhs.m_texture;
      m_position = rhs.m_position;
      m_size = rhs.m_size;
      m_color = rhs.m_color;
      m_rotation = rhs.m_rotation;
    }

    return *this;
  }

  Sprite &Sprite::operator=( Sprite &&rhs ) noexcept {
    if ( this != &rhs ) {

      delete m_texture;

      m_texture = rhs.m_texture;
      m_position = rhs.m_position;
      m_size = rhs.m_size;
      m_color = rhs.m_color;
      m_rotation = rhs.m_rotation;

      rhs.m_texture = nullptr;
    }

    return *this;
  }

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

  Texture2D *Sprite::getTexture() const noexcept {
    return m_texture;
  }

}    // namespace nile

