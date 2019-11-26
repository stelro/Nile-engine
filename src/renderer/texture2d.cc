/* ================================================================================
$File: texture2d.cc
$Date: $
$Revision: $
$Creator: Rostislav Orestis Stelmach
$Notice: $
================================================================================ */

#include "Nile/renderer/texture2d.hh"

#include <GL/glew.h>

namespace nile {

  Texture2D::Texture2D() noexcept
      : m_width( 0 )
      , m_height( 0 )
      , m_internalFormat( GL_RGB )
      , m_imageFormat( GL_RGB )
      , m_wrapS( GL_REPEAT )
      , m_wrapT( GL_REPEAT )
      , m_filterMin( GL_LINEAR )
      , m_filterMax( GL_LINEAR ) {
    glGenTextures( 1, &this->m_id );
  }

  void Texture2D::generate( u32 width, u32 height, unsigned char *data ) noexcept {

    this->m_width = width;
    this->m_height = height;

    // create texture
    glBindTexture( GL_TEXTURE_2D, this->m_id );
    glTexImage2D( GL_TEXTURE_2D, 0, this->m_internalFormat, width, height, 0, this->m_imageFormat,
                  GL_UNSIGNED_BYTE, data );

    // Set texture wrap and filter modes
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, this->m_wrapS );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, this->m_wrapT );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, this->m_filterMin );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, this->m_filterMax );


    // unbind texture
    glBindTexture( GL_TEXTURE_2D, 0 );
  }

  void Texture2D::bind() const noexcept {
    glBindTexture( GL_TEXTURE_2D, this->m_id );
  }

  void Texture2D::unbind() const noexcept {
    glBindTexture( GL_TEXTURE_2D, 0 );
  }

  void Texture2D::setParameter(TextureTargetParams target, TextureParams param) noexcept {
    this->bind();
    glTexParameteri(GL_TEXTURE_2D, GlTargetParmas[static_cast<GLenum>(target)], GlParams[static_cast<u32>(param)]);
  }


}    // namespace nile
