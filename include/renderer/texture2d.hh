#pragma once

#include "core/nile.hh"

namespace nile {

  // Texture2D is able to store and configure texture in OpenGL.
  // It also hosts utility funcitons for easy managment.

  class Texture2D {
  private:
    // Holds the ID of the texture object, used for all texture operations to
    // reference this particular texture
    u32 m_id;

    // texture image dimensions
    u32 m_width, m_height;

    // texture format, format of the texture object
    u32 m_internalFormat;
    // texture format, format of the loaded image
    u32 m_imageFormat;

    // texture configuration
    u32 m_wrapS;
    u32 m_wrapT;
    u32 m_filterMin;
    u32 m_filterMax;

  public:
    Texture2D() noexcept;

    // Generates texture from image data
    void generate( u32 width, u32 height, unsigned char *data ) noexcept;

    // Binds the texture as the current active GL_TEXTURE_2D texture object
    void bind() const noexcept;

    // Getters
    [[nodiscard]] inline u32 getID() const noexcept {
      return m_id;
    }

    [[nodiscard]] inline u32 getWidth() const noexcept {
      return m_width;
    }

    [[nodiscard]] inline u32 getHeight() const noexcept {
      return m_height;
    }

    // Setters
    void setInternalFormat( u32 format ) noexcept {
      m_internalFormat = format;
    }

    void setImageFormat( u32 format ) noexcept {
      m_imageFormat = format;
    }
  };

}    // namespace nile
