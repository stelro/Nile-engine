#include "Nile/renderer/render_primitive_system.hh"
#include "Nile/ecs/components/primitive.hh"
#include "Nile/ecs/components/renderable.hh"
#include "Nile/ecs/components/transform.hh"
#include "Nile/ecs/ecs_coordinator.hh"
#include "Nile/renderer/shaderset.hh"

#include <GL/glew.h>
#include <glm/gtc/matrix_transform.hpp>

namespace nile {

  RenderPrimitiveSystem::RenderPrimitiveSystem( const std::shared_ptr<Coordinator> &coordinator,
                                                const std::shared_ptr<ShaderSet> &shader ) noexcept
      : m_ecsCoordinator( coordinator )
      , m_primShader( shader ) {}

  void RenderPrimitiveSystem::create() noexcept {
    this->initRenderData();
  }

  void RenderPrimitiveSystem::destroy() noexcept {}

  void RenderPrimitiveSystem::render( float dt ) noexcept {

    for ( const auto &entity : m_entities ) {
      auto &transform = m_ecsCoordinator->getComponent<Transform>( entity );
      auto &renderable = m_ecsCoordinator->getComponent<Renderable>( entity );
      auto &primitive = m_ecsCoordinator->getComponent<Primitive>( entity );

      this->m_primShader->use();

      // f32 vertices[] = {primitive.begin.x, primitive.begin.y, primitive.end.x, primitive.end.y};
      //
      // glBindBuffer( GL_ARRAY_BUFFER, primitive.vbo );
      // glBufferSubData( GL_ARRAY_BUFFER, 0, sizeof( vertices ), vertices );
      // glBindBuffer( GL_ARRAY_BUFFER, 0 );
      //
      glm::mat4 model = glm::mat4 {1.0f};
      model = glm::translate( model, transform.position );
      model =
          glm::rotate( model, glm::radians( transform.xRotation ), glm::vec3( 1.0f, 0.0f, 0.0f ) );
      model =
          glm::rotate( model, glm::radians( transform.yRotation ), glm::vec3( 0.0f, 1.0f, 0.0f ) );
      model =
          glm::rotate( model, glm::radians( transform.zRotation ), glm::vec3( 0.0f, 0.0f, 1.0f ) );


      this->m_primShader->SetMatrix4( "model", model );
      this->m_primShader->SetVector3f( "primitive_color", renderable.color );

      glLineWidth( primitive.lineWidth );
      glBindVertexArray( primitive.vao );
      glDrawArrays( GL_LINES, 0, 2 );
      glBindVertexArray( 0 );
    }
  }

  void RenderPrimitiveSystem::initRenderData() noexcept {


    for ( const auto &entity : m_entities ) {
      auto &primitive = m_ecsCoordinator->getComponent<Primitive>( entity );
      // Set the begin and end from our primitive component
      const f32 vertices[] = {primitive.begin.x, primitive.begin.y, primitive.end.x,
                              primitive.end.y};
      glGenVertexArrays( 1, &primitive.vao );
      glGenBuffers( 1, &primitive.vbo );
      glBindBuffer( GL_ARRAY_BUFFER, primitive.vbo );
      glBufferData( GL_ARRAY_BUFFER, sizeof( vertices ), vertices, GL_STATIC_DRAW );
      glBindVertexArray( primitive.vao );
      glEnableVertexAttribArray( 0 );
      glVertexAttribPointer( 0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof( f32 ), ( void * )0 );
      glBindBuffer( GL_ARRAY_BUFFER, 0 );
      glBindVertexArray( 0 );
    }
  }


}    // namespace nile
