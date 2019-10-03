/* ================================================================================
$File: font_renderer.cc
$Date: $
$Revision: $
$Creator: Rostislav Orestis Stelmach
$Notice: $
================================================================================ */

#include "Nile/renderer/font_renderer.hh"
#include "Nile/core/logger.hh"
#include "Nile/core/settings.hh"
#include "Nile/renderer/shaderset.hh"

#include <GL/glew.h>
#include <glm/gtc/matrix_transform.hpp>

namespace nile {

  FontRenderer::FontRenderer( ShaderSet* shader,
                              const std::shared_ptr<Settings> &settings,
                              const std::string &fontName, u32 fontSize ) noexcept
      : m_shader( shader )
      , m_settings( settings ) {
    // Each of FreeType functions returns a non-zero integer whenever an error occured.

    if ( FT_Init_FreeType( &m_fontLib ) ) {
      log::error( "Could not init FreeType Library!\n" );
    }

    if ( FT_New_Face( m_fontLib, fontName.c_str(), 0, &m_fontFace ) ) {
      log::error( "Failed to load font %s\n", fontName.c_str() );
    }

    // Setting the width ( 2nd parameter ) to 0, lets the face dynamically calculate the width
    // based on the given height
    FT_Set_Pixel_Sizes( m_fontFace, 0, fontSize );

    this->init();
  }

  FontRenderer::~FontRenderer() noexcept {
    FT_Done_Face( m_fontFace );
    FT_Done_FreeType( m_fontLib );
  }

  void FontRenderer::init() noexcept {

    m_shader->SetMatrix4( "projection",
                          glm::ortho( 0.0f, static_cast<f32>( m_settings->getWidth() ),
                                      static_cast<f32>( m_settings->getHeight() ), 0.0f ),
                          GL_TRUE );

    m_shader->SetInteger( "text", 0 );

    // Initialize the vertex array object
    glGenVertexArrays( 1, &m_vao );
    glGenBuffers( 1, &m_vbo );
    glBindVertexArray( m_vao );
    glBindBuffer( GL_ARRAY_BUFFER, m_vbo );
    glBufferData( GL_ARRAY_BUFFER, sizeof( f32 ) * 6 * 4, nullptr, GL_DYNAMIC_DRAW );
    glEnableVertexAttribArray( 0 );
    glVertexAttribPointer( 0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof( f32 ), 0 );
    glBindBuffer( GL_ARRAY_BUFFER, 0 );
    glBindVertexArray( 0 );

    // Disable byte-alignment restriction
    glPixelStorei( GL_UNPACK_ALIGNMENT, 1 );

    for ( unsigned char c = 0; c < 128; c++ ) {

      // Load character glyph
      if ( FT_Load_Char( m_fontFace, c, FT_LOAD_RENDER ) ) {
        log::error( "Faild to load glyph [%c]\n", c );
        continue;
      }

      // Generate the texture
      u32 texture;
      glGenTextures( 1, &texture );
      glBindTexture( GL_TEXTURE_2D, texture );
      glTexImage2D( GL_TEXTURE_2D, 0, GL_RED, m_fontFace->glyph->bitmap.width,
                    m_fontFace->glyph->bitmap.rows, 0, GL_RED, GL_UNSIGNED_BYTE,
                    m_fontFace->glyph->bitmap.buffer );
      // Set texture options
      glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
      glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
      glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
      glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );

      // Store the character of later use
      Character character = {
          texture, glm::ivec2( m_fontFace->glyph->bitmap.width, m_fontFace->glyph->bitmap.rows ),
          glm::ivec2( m_fontFace->glyph->bitmap_left, m_fontFace->glyph->bitmap_top ),
          static_cast<u32>( m_fontFace->glyph->advance.x )};

      m_characters.insert( std::pair<char, Character>( c, character ) );
    }
  }

  void FontRenderer::renderText( const std::string &text, f32 x, f32 y, f32 scale,
                                 const glm::vec3 &color ) noexcept {

    m_shader->use();
    m_shader->SetVector3f( "textColor", color );
    glActiveTexture( GL_TEXTURE0 );
    glBindVertexArray( this->m_vao );

    // Iterate through all characters
    std::string::const_iterator c;
    for ( c = text.begin(); c != text.end(); c++ ) {
      Character ch = m_characters[ *c ];

      f32 xpos = x + ch.bearing.x * scale;
      f32 ypos = y + ( this->m_characters[ 'H' ].bearing.y - ch.bearing.y ) * scale;

      f32 w = ch.size.x * scale;
      f32 h = ch.size.y * scale;

      // Update vbo for each character
      f32 vertices[ 6 ][ 4 ] = {

          {xpos, ypos + h, 0.0f, 1.0f}, {xpos + w, ypos, 1.0f, 0.0f},
          {xpos, ypos, 0.0f, 0.0f},

          {xpos, ypos + h, 0.0f, 1.0f}, {xpos + w, ypos + h, 1.0f, 1.0f},
          {xpos + w, ypos, 1.0f, 0.0f}};

      glBindTexture( GL_TEXTURE_2D, ch.textureID );
      glBindBuffer( GL_ARRAY_BUFFER, m_vbo );
      glBufferSubData( GL_ARRAY_BUFFER, 0, sizeof( vertices ), vertices );
      glBindBuffer( GL_ARRAY_BUFFER, 0 );
      glDrawArrays( GL_TRIANGLES, 0, 6 );

      x += ( ch.advance >> 6 ) * scale;
    }

    glBindVertexArray( 0 );
    glBindTexture( GL_TEXTURE_2D, 0 );
  }


}    // namespace nile
