/* ================================================================================
$File: font_loader.cc
$Date: 28 Nov 2019 $
$Revision: $
$Creator: Rostislav Orestis Stelmach
$Notice: $
================================================================================ */

#include "Nile/asset/subsystem/font_loader.hh"
#include "Nile/log/log.hh"

namespace nile {

  std::shared_ptr<Font> AssetLoader<Font>::operator()( const std::string &fileName,
                                                       const std::string &filePath ) noexcept {

    auto font = std::make_shared<Font>();

    // Each of FreeType functions, returns a non-zero integer whenever an error has
    // occured.

    // @fix: instead of "throw" errors in the loader functor, return then
    // somehow
    auto error = FT_Init_FreeType( &font->fontLibrary );

    if ( error )
      log::error( "Could not initialize FreeType library\n" );

    error = FT_New_Face( font->fontLibrary, filePath.c_str(), 0, &font->fontFace );

    if ( error == FT_Err_Unknown_File_Format )
      log::error( "Tried to load font %s\n%s\n", filePath.c_str(),
                  "It appears that the specific font format is unsupported" );
    else if ( error )
      log::error( "Tried to load font %s\n%s\n", filePath.c_str(),
                  "The specific font could not be opened or read, seems that it is broken or not found" );

    return font;
  }

}    // namespace nile
