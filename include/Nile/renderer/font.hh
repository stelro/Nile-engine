#pragma once

#include "Nile/asset/asset.hh"
#include "Nile/core/types.hh"

#include <ft2build.h>
#include FT_FREETYPE_H

#include <string>

namespace nile {

  struct Font : public Asset {
    FT_Face fontFace;
    std::string fontName;
    u32 fontSize;
  };

}    // namespace nile
