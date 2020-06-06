/* ================================================================================
$File: texture2d.hh
$Date: $
$Revision: $
$Creator: Rostislav Orestis Stelmach
$Notice: $
================================================================================ */

#pragma once

#include "Nile/asset/asset.hh"
#include "Nile/core/types.hh"
#include <GL/glew.h>

namespace nile {

  // Texture2D is able to store and configure texture in OpenGL.
  // It also hosts utility funcitons for easy managment.

  enum class TextureType : u32 {
    DIFFUSE = 0,
    SPECULAR,
    NORMAL,
    AMBIENT,
    EMISSIVE,
    TRANSPARENT,
    NONE
  };

  enum class TextureTargetParams : u16 {
    TEXTURE_MIN_FILTER = 0,
    TEXTURE_MAG_FILTER,
    TEXTURE_WRAP_S,
    TEXTURE_WRAP_T,
  };

  enum class TextureParams : u32 {
    NEAREST = 0,
    LINEAR,
    NEAREST_MIPMAP_NEAREST,
    LINEAR_MIPMAP_NEAREST,
    NEAREST_MIPMAP_LINEAR,
    LINEAR_MIPMAP_LINEAR,
    REPEAT,
    MIRRORED,
    CLAMP_TO_EDGE,
    CLAMP_TO_BORDER,
  };

  const constexpr GLenum GlTargetParmas[ 4 ] = {
      GL_TEXTURE_MIN_FILTER,
      GL_TEXTURE_MAG_FILTER,
      GL_TEXTURE_WRAP_S,
      GL_TEXTURE_WRAP_T,
  };

  const constexpr GLint GlParams[ 10 ] = {GL_NEAREST,
                                          GL_LINEAR,
                                          GL_NEAREST_MIPMAP_NEAREST,
                                          GL_LINEAR_MIPMAP_NEAREST,
                                          GL_NEAREST_MIPMAP_LINEAR,
                                          GL_LINEAR_MIPMAP_LINEAR,
                                          GL_REPEAT,
                                          GL_MIRRORED_REPEAT,
                                          GL_CLAMP_TO_EDGE,
                                          GL_CLAMP_TO_BORDER};

  inline std::string TextureTypeStr( TextureType type ) {
    switch ( type ) {
      case TextureType::DIFFUSE:
        return "diffuse";
      case TextureType::SPECULAR:
        return "specular";
      case TextureType::NORMAL:
        return "normal";
      case TextureType::AMBIENT:
        return "ambient";
      case TextureType::EMISSIVE:
        return "emissive";
      default:
        return "none";
    }
  }

  class Texture2D : public Asset {
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

    TextureType m_textureType = TextureType::DIFFUSE;

    unsigned char* m_imageData;

  public:
    Texture2D() noexcept;

    // Generates texture from image data
    void generate( u32 width, u32 height, unsigned char *data ) noexcept;

    // Binds the texture as the current active GL_TEXTURE_2D texture object
    void bind() const noexcept;

    // Unbind the texture
    void unbind() const noexcept;

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

    [[nodiscard]] inline TextureType getTextureType() const noexcept {
      return m_textureType;
    }

    // Setters
    void setInternalFormat( u32 format ) noexcept {
      m_internalFormat = format;
    }

    void setImageFormat( u32 format ) noexcept {
      m_imageFormat = format;
    }

    void setTexturetype( TextureType type ) noexcept {
      m_textureType = type;
    }

    void setParameter(TextureTargetParams target, TextureParams param) noexcept;

    unsigned char* getData() const noexcept {
      return m_imageData;
    }

  };

}    // namespace nile
