#pragma once

#include "Nile/core/nile.hh"

#include <ft2build.h>
#include <glm/glm.hpp>
#include <string>
#include <map>
#include <memory>
#include FT_FREETYPE_H

namespace nile {

  class Shader;
  class Settings;

  struct Character {
    // ID handle of the glyph texture
    u32 textureID;
    // Size of glyph
    glm::ivec2 size;
    // Offset from baseline to left/top of the glyph
    glm::ivec2 bearing;
    // offset to advance to the next glyph
    u32 advance;
  };

  class FontRenderer {
  private:
    std::shared_ptr<Shader> m_shader;
    FT_Library m_fontLib;
    FT_Face m_fontFace;
    
    std::map<char, Character> m_characters;

    u32 m_vao;
    u32 m_vbo;

    std::shared_ptr<Settings> m_settings;

    void init() noexcept;

  public:
    FontRenderer(const std::shared_ptr<Shader>& shader,const std::shared_ptr<Settings>& settings, const std::string &fontName, u32 fontSize ) noexcept;
    ~FontRenderer() noexcept;
    
    void renderText(const std::string& text, f32 x, f32 y, f32 scale, const glm::vec3& color) noexcept;
  };

}    // namespace nile
