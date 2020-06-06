/* ================================================================================
$File: texture_loader.cc
$Date: $
$Revision: $
$Creator: Rostislav Orestis Stelmach
$Notice: $
================================================================================ */

#include "Nile/asset/subsystem/texture_loader.hh"
#include "Nile/log/log.hh"
#include "Nile/renderer/texture2d.hh"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <GL/glew.h>


namespace nile {

  std::shared_ptr<Texture2D>
  AssetLoader<Texture2D>::operator()( const std::string &assetName,
                                      const std::string &filePath ) noexcept {

    auto texture = std::make_shared<Texture2D>();

    // @incomplete
    // @incomplete
    // @incomplete
    // this should be handeled as game engine option or parameter
    const auto alpha = true;

    if ( alpha ) {
      texture->setInternalFormat( GL_RGBA );
      texture->setImageFormat( GL_RGBA );
    }

    // Load image
    int width, height, nrChannels;
    stbi_uc *image = nullptr;
    image = stbi_load( filePath.data(), &width, &height, &nrChannels, STBI_rgb_alpha );

    // We flip the y-axis coordinate, because open-gl expects the 0.0 coordinate to
    // be on the bottom side of the image, but usually images have 0.0 at the top of
    // the y-axis
    //stbi_set_flip_vertically_on_load( true );

    if ( !image ) {
      log::error( "Failed to load texture { %s }\n", filePath.data() );
    }

    texture->generate( width, height, image );
    stbi_image_free( image );

    texture->setAssetName( assetName );
    texture->setFileName( filePath );

    return texture;
  }

}    // namespace nile
