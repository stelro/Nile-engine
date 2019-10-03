/* ================================================================================
$File: texture_loader.cc
$Date: $
$Revision: $
$Creator: Rostislav Orestis Stelmach
$Notice: $
================================================================================ */

#include "Nile/asset/subsystem/texture_loader.hh"
#include "Nile/renderer/texture2d.hh"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <GL/glew.h>

namespace nile {

  TextureLoader::TextureLoader( bool alpha ) noexcept
      : m_alpha( alpha ) {}

  Asset *TextureLoader::loadAsset( const std::string &assetName,
                                   const std::string &filePath ) noexcept {

    Texture2D *texture = new Texture2D();

    if ( m_alpha ) {
      texture->setInternalFormat( GL_RGBA );
      texture->setImageFormat( GL_RGBA );
    }

    // Load image
    int width, height, nrChannels;
    unsigned char *image = nullptr;
    image = stbi_load( filePath.data(), &width, &height, &nrChannels, STBI_rgb_alpha );

    if ( !image ) {
      log::error( "Failed to load texture { %s }\n", filePath.data() );
    }

    texture->generate( width, height, image );
    stbi_image_free( image );

    texture->setAssetName( assetName );
    texture->setFileName( filePath );

    return texture;
  }

  void TextureLoader::unloadAsset( Asset *asset ) noexcept {}


}    // namespace nile
