#pragma once

#include "Nile/core/types.hh"
#include "Nile/utils/font_character.hh"

#include <map>
#include <memory>
#include <string>

namespace nile {

  class Font;

  struct FontComponent {
    std::shared_ptr<Font> font;
    std::map<char, FontCharacter> characters;
    std::string text;
    u32 fontSize {18};
    u32 vao;
    u32 vbo;
    u32 width {1};
    u32 height {1};
  };

}    // namespace nile
