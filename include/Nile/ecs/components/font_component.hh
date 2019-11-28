#pragma once

#include "Nile/core/types.hh"
#include "Nile/utils/font_character.hh"

#include <map>
#include <string>
#include <memory>

namespace nile {

  class Font;

  struct FontComponent {
    std::shared_ptr<Font> font;
    std::map<char, FontCharacter> characters;
    std::string text;
    u32 fontSize = 18;
    u32 vao;
    u32 vbo;
  };

}    // namespace nile
