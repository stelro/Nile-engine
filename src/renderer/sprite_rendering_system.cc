/* ================================================================================
$File: sprite_rendering_system.cc $Date: 09 Oct 2019 $
$Revision: $
$Creator: Rostislav Orestis Stelmach
$Notice: $
================================================================================ */

#include "Nile/renderer/sprite_rendering_system.hh"
#include "Nile/ecs/components/renderable.hh"
#include "Nile/ecs/components/sprite.hh"
#include "Nile/ecs/components/transform.hh"
#include "Nile/ecs/ecs_coordinator.hh"
#include "Nile/renderer/shaderset.hh"
#include "Nile/renderer/texture2d.hh"

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace nile {

  SpriteRenderingSystem::SpriteRenderingSystem( const std::shared_ptr<Coordinator> &coordinator,
                                                const std::shared_ptr<ShaderSet> &shader ) noexcept
      : m_ecsCoordinator( coordinator )
      , m_spriteShader( shader ) {}

  void SpriteRenderingSystem::create() noexcept {
    this->initRenderData();
  }

  void SpriteRenderingSystem::destroy() noexcept {
    glDeleteVertexArrays( 1, &this->m_quadVAO );
  }

  void SpriteRenderingSystem::update( float dt ) noexcept {}

  void SpriteRenderingSystem::render( float dt ) noexcept {

    for ( const auto &entity : m_entities ) {
      auto &transform = m_ecsCoordinator->getComponent<Transform>( entity );
      auto &renderable = m_ecsCoordinator->getComponent<Renderable>( entity );
      auto &sprite = m_ecsCoordinator->getComponent<SpriteComponent>( entity );

      this->m_spriteShader->use();

      glm::mat4 model = glm::mat4( 1.0f );
      model = glm::translate( model, transform.position );

      // Because we specified the quad's vertices with (0,0) as the top-left coordinate
      // of the quad, all rotations witll rotate around this point of (0.0). Basically the
      // origin of rotation is the top-left of the quad which produces undesirable results.
      // so we move the origin of rotation to the center of the quad, so the quad
      // neatly rotates around this origin.
      // model = glm::translate(
      //     model, glm::vec3( 0.5f * transform.scale.x, 0.5f * transform.scale.y, 0.0f ) );

      model =
          glm::rotate( model, glm::radians( transform.xRotation ), glm::vec3( 1.0f, 0.0f, 0.0f ) );
      model =
          glm::rotate( model, glm::radians( transform.yRotation ), glm::vec3( 0.0f, 1.0f, 0.0f ) );
      model =
          glm::rotate( model, glm::radians( transform.zRotation ), glm::vec3( 0.0f, 0.0f, 1.0f ) );

      // model = glm::translate(
      //     model, glm::vec3( -0.5f * transform.scale.x, -0.5f * transform.scale.y, 0.0f ) );
      //
      model = glm::scale( model, glm::vec3( transform.scale.x, transform.scale.y, 1.0f ) );

      this->m_spriteShader->SetMatrix4( "model", model );
      this->m_spriteShader->SetVector3f( "objectColor", renderable.color );

      glActiveTexture( GL_TEXTURE0 );
      sprite.texture->bind();

      glBindVertexArray( this->m_quadVAO );
      glDrawArrays( GL_TRIANGLES, 0, 6 );
      glBindVertexArray( 0 );
    }
  }

  void SpriteRenderingSystem::initRenderData() noexcept {
    u32 vbo;
    f32 vertices[] = {0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f,
                      0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
                      0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f,
                      0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f};

    log::print("Sprite rendering system\n");

    glGenVertexArrays( 1, &this->m_quadVAO );
    glGenBuffers( 1, &vbo );
    // pos
    // normal
    // texture

    glBindBuffer( GL_ARRAY_BUFFER, vbo );
    glBufferData( GL_ARRAY_BUFFER, sizeof( vertices ), vertices, GL_STATIC_DRAW );

    glBindVertexArray( this->m_quadVAO );

    // Position
    glEnableVertexAttribArray( 0 );
    glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof( f32 ), ( void * )0 );

    // Normal
    glEnableVertexAttribArray( 1 );
    glVertexAttribPointer( 1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof( f32 ),
                           ( void * )( 3 * sizeof( f32 ) ) );

    // UV
    glEnableVertexAttribArray( 2 );
    glVertexAttribPointer( 2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof( f32 ),
                           ( void * )( 6 * sizeof( f32 ) ) );

    glBindBuffer( GL_ARRAY_BUFFER, 0 );
    glBindVertexArray( 0 );
  }
}    // namespace nile
