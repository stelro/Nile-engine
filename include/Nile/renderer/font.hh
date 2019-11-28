#pragma once

#include "Nile/asset/asset.hh"
#include "Nile/core/types.hh"

#include <ft2build.h>
#include FT_FREETYPE_H

#include <string>

namespace nile {

  struct Font : public Asset {
    // Handle to the face object
    FT_Face fontFace;
    // Handle to the library
    FT_Library fontLibrary;
    // Font name
    std::string fontName;
  };

}    // namespace nile
