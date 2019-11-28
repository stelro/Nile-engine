#pragma once

#include "Nile/core/types.hh"
#include <glm/glm.hpp>

namespace nile {

  // This struct actually represents a font glyp
  // ( individual sprite that represents a character for the
  // specific font)
  struct FontCharacter {
    // ID handle of the glyph
    u32 textureID;
    // Size of a glyph
    glm::ivec2 size;
    // Offset from baseline left/top of the glyp
    glm::ivec2 bearing;
    // Offset to advance to the next glyph
    u32 advance;
  };


}    // namespace nile
