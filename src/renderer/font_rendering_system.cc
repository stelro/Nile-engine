#include "Nile/renderer/font_rendering_system.hh"
#include "Nile/core/assert.hh"
#include "Nile/core/settings.hh"
#include "Nile/ecs/components/font_component.hh"
#include "Nile/ecs/components/renderable.hh"
#include "Nile/ecs/components/transform.hh"
#include "Nile/ecs/ecs_coordinator.hh"
#include "Nile/log/log.hh"
#include "Nile/renderer/font.hh"
#include "Nile/renderer/shaderset.hh"
#include "Nile/utils/font_character.hh"

#include <GL/glew.h>
#include <glm/gtc/matrix_transform.hpp>

namespace nile {

  FontRenderingSystem::FontRenderingSystem( const std::shared_ptr<Coordinator> &coordinator,
                                            const std::shared_ptr<Settings> &settings,
                                            const std::shared_ptr<ShaderSet> &shader ) noexcept
      : m_ecsCoordinator( coordinator )
      , m_settings( settings )
      , m_fontShader( shader ) {}

  void FontRenderingSystem::initRenderData() noexcept {

    for ( const auto &entity : m_entities ) {

      auto &font = m_ecsCoordinator->getComponent<FontComponent>( entity );

      ASSERT_M( font.font, "Font field in FontComponent is not initialized or it's empty!" );

      FT_Set_Pixel_Sizes( font.font->fontFace, 0, font.fontSize );

      m_fontShader->SetMatrix4( "projection",
                                glm::ortho( 0.0f, static_cast<f32>( m_settings->getWidth() ),
                                            static_cast<f32>( m_settings->getHeight() ), 0.0f ),
                                GL_TRUE );

      m_fontShader->SetInteger( "text", 0 );

      // Initialize the vertex array object
      glGenVertexArrays( 1, &font.vao );
      glGenBuffers( 1, &font.vbo );
      glBindVertexArray( font.vao );
      glBindBuffer( GL_ARRAY_BUFFER, font.vbo );
      glBufferData( GL_ARRAY_BUFFER, sizeof( f32 ) * 6 * 4, nullptr, GL_DYNAMIC_DRAW );
      glEnableVertexAttribArray( 0 );
      glVertexAttribPointer( 0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof( f32 ), 0 );
      glBindBuffer( GL_ARRAY_BUFFER, 0 );
      glBindVertexArray( 0 );

      // Disable byte-alignment restriction
      glPixelStorei( GL_UNPACK_ALIGNMENT, 1 );

      for ( unsigned char c = 0; c < 128; c++ ) {

        // Load character glyph
        if ( FT_Load_Char( font.font->fontFace, c, FT_LOAD_RENDER ) ) {
          log::error( "Faild to load glyph [%c]\n", c );
          continue;
        }

        // Generate the texture
        u32 texture;
        glGenTextures( 1, &texture );
        glBindTexture( GL_TEXTURE_2D, texture );
        glTexImage2D( GL_TEXTURE_2D, 0, GL_RED, font.font->fontFace->glyph->bitmap.width,
                      font.font->fontFace->glyph->bitmap.rows, 0, GL_RED, GL_UNSIGNED_BYTE,
                      font.font->fontFace->glyph->bitmap.buffer );
        // Set texture options
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );

        // Store the character of later use
        FontCharacter character = {texture,
                                   glm::ivec2( font.font->fontFace->glyph->bitmap.width,
                                               font.font->fontFace->glyph->bitmap.rows ),
                                   glm::ivec2( font.font->fontFace->glyph->bitmap_left,
                                               font.font->fontFace->glyph->bitmap_top ),
                                   static_cast<u32>( font.font->fontFace->glyph->advance.x )};

        font.characters.insert( std::pair<char, FontCharacter>( c, character ) );
      }
    }
  }

  void FontRenderingSystem::create() noexcept {
    this->initRenderData();
  }

  void FontRenderingSystem::destroy() noexcept {}
  void FontRenderingSystem::update( float dt ) noexcept {}
  void FontRenderingSystem::render( float dt ) noexcept {

    for ( const auto &entity : m_entities ) {

      auto font = m_ecsCoordinator->getComponent<FontComponent>( entity );
      auto renderable = m_ecsCoordinator->getComponent<Renderable>( entity );
      auto transform = m_ecsCoordinator->getComponent<Transform>( entity );

      m_fontShader->use();
      m_fontShader->SetVector3f( "textColor", renderable.color );
      glActiveTexture( GL_TEXTURE0 );
      glBindVertexArray( font.vao );

      // Iterate through all characters
      std::string::const_iterator c;
      for ( c = font.text.begin(); c != font.text.end(); c++ ) {
        FontCharacter ch = font.characters[ *c ];

        f32 xpos = transform.position.x + ch.bearing.x * transform.scale.x;
        f32 ypos = transform.position.y +
                   ( font.characters[ 'H' ].bearing.y - ch.bearing.y ) * transform.scale.y;

        f32 w = ch.size.x * transform.scale.x;
        f32 h = ch.size.y * transform.scale.y;

        // Update vbo for each character
        f32 vertices[ 6 ][ 4 ] = {

            {xpos, ypos + h, 0.0f, 1.0f}, {xpos + w, ypos, 1.0f, 0.0f},
            {xpos, ypos, 0.0f, 0.0f},

            {xpos, ypos + h, 0.0f, 1.0f}, {xpos + w, ypos + h, 1.0f, 1.0f},
            {xpos + w, ypos, 1.0f, 0.0f}};

        glBindTexture( GL_TEXTURE_2D, ch.textureID );
        glBindBuffer( GL_ARRAY_BUFFER, font.vbo );
        glBufferSubData( GL_ARRAY_BUFFER, 0, sizeof( vertices ), vertices );
        glBindBuffer( GL_ARRAY_BUFFER, 0 );
        glDrawArrays( GL_TRIANGLES, 0, 6 );

        transform.position.x += ( ch.advance >> 6 ) * transform.scale.x;
      }

      glBindVertexArray( 0 );
      glBindTexture( GL_TEXTURE_2D, 0 );
    }
  }

}    // namespace nile
