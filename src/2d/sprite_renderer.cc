/* ================================================================================
$File: sprite_renderer.cc
$Date: $
$Revision: $
$Creator: Rostislav Orestis Stelmach
$Notice: $
================================================================================ */

#include "Nile/2d/sprite_renderer.hh"
#include "Nile/renderer/shaderset.hh"
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace nile {

  SpriteRenderer::SpriteRenderer( ShaderSet *shader ) noexcept
      : m_shader( shader ) {
    this->initRenderData();
  }

  SpriteRenderer::~SpriteRenderer() noexcept {
    glDeleteVertexArrays( 1, &this->m_quadVAO );
  }

  void SpriteRenderer::draw( Texture2D *texture, glm::vec2 position, glm::vec2 size, f32 rotate,
                             glm::vec3 color ) noexcept {

    this->m_shader->use();

    glm::mat4 model = glm::mat4( 1.0f );
    model = glm::translate( model, glm::vec3( position, 0.0f ) );

    // Because we specified the quad's vertices with (0,0) as the top-left coordinate
    // of the quad, all rotations witll rotate around this point of (0.0). Basically the
    // origin of rotation is the top-left of the quad which produces undesirable results.
    // so we move the origin of rotation to the center of the quad, so the quad
    // neatly rotates around this origin.
    model = glm::translate( model, glm::vec3( 0.5f * size.x, 0.5f * size.y, 0.0f ) );
    model = glm::rotate( model, rotate, glm::vec3( 0.0f, 0.0f, 1.0f ) );
    model = glm::translate( model, glm::vec3( -0.5f * size.x, -0.5f * size.y, 0.0f ) );

    model = glm::scale( model, glm::vec3( size, 1.0f ) );

    this->m_shader->SetMatrix4( "model", model );
    this->m_shader->SetVector3f( "spriteColor", color );

    glActiveTexture( GL_TEXTURE0 );
    texture->bind();

    glBindVertexArray( this->m_quadVAO );
    glDrawArrays( GL_TRIANGLES, 0, 6 );
    glBindVertexArray( 0 );
  }

  void SpriteRenderer::draw( const Sprite &sprite ) noexcept {
    // @cleanup!
    this->draw( sprite.getTexture(), sprite.getPosition(), sprite.getSize(), sprite.getRotation(),
                sprite.getColor() );
  }

  void SpriteRenderer::initRenderData() noexcept {
    u32 vbo;
    f32 vertices[] = {0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,

                      0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f};

    glGenVertexArrays( 1, &this->m_quadVAO );
    glGenBuffers( 1, &vbo );

    glBindBuffer( GL_ARRAY_BUFFER, vbo );
    glBufferData( GL_ARRAY_BUFFER, sizeof( vertices ), vertices, GL_STATIC_DRAW );

    glBindVertexArray( this->m_quadVAO );
    glEnableVertexAttribArray( 0 );
    glVertexAttribPointer( 0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof( f32 ), ( void * )0 );
    glBindBuffer( GL_ARRAY_BUFFER, 0 );
    glBindVertexArray( 0 );
  }

}    // namespace nile
