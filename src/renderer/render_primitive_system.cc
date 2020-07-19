#include "Nile/renderer/render_primitive_system.hh"
#include "Nile/ecs/components/primitive.hh"
#include "Nile/ecs/components/renderable.hh"
#include "Nile/ecs/components/transform.hh"
#include "Nile/ecs/ecs_coordinator.hh"
#include "Nile/renderer/shaderset.hh"

#include <GL/glew.h>
#include <glm/gtc/matrix_transform.hpp>
#include <spdlog/spdlog.h>

namespace nile {

  RenderPrimitiveSystem::RenderPrimitiveSystem( const std::shared_ptr<Coordinator> &coordinator,
                                                const std::shared_ptr<ShaderSet> &shader ) noexcept
      : ecs_coordinator_( coordinator )
      , primitive_shader_( shader ) {}

  void RenderPrimitiveSystem::create() noexcept {
    this->init_rendering_data();
    spdlog::info(
        "ECS RenderPrimitiveSystem has been registered to ECS manager and created successfully." );
  }

  void RenderPrimitiveSystem::destroy() noexcept {}

  void RenderPrimitiveSystem::render( float dt ) noexcept {

    for ( const auto &entity : entities_ ) {
      auto &transform = ecs_coordinator_->getComponent<Transform>( entity );
      auto &renderable = ecs_coordinator_->getComponent<Renderable>( entity );
      auto &primitive = ecs_coordinator_->getComponent<Primitive>( entity );

      this->primitive_shader_->use();

      // f32 vertices[] = {primitive.begin.x, primitive.begin.y, primitive.end.x, primitive.end.y};
      //
      // glBindBuffer( GL_ARRAY_BUFFER, primitive.vbo );
      // glBufferSubData( GL_ARRAY_BUFFER, 0, sizeof( vertices ), vertices );
      // glBindBuffer( GL_ARRAY_BUFFER, 0 );
      //
      glm::mat4 model = glm::mat4 { 1.0f };
      model = glm::translate( model, transform.position );
      model =
          glm::rotate( model, glm::radians( transform.xRotation ), glm::vec3( 1.0f, 0.0f, 0.0f ) );
      model =
          glm::rotate( model, glm::radians( transform.yRotation ), glm::vec3( 0.0f, 1.0f, 0.0f ) );
      model =
          glm::rotate( model, glm::radians( transform.zRotation ), glm::vec3( 0.0f, 0.0f, 1.0f ) );


      this->primitive_shader_->SetMatrix4( "model", model );
      this->primitive_shader_->SetVector3f( "primitive_color", renderable.color );

      glLineWidth( primitive.lineWidth );
      glBindVertexArray( primitive.vao );
      glDrawArrays( GL_LINES, 0, 2 );
      glBindVertexArray( 0 );
    }
  }

  void RenderPrimitiveSystem::init_rendering_data() noexcept {

    for ( const auto &entity : entities_ ) {
      auto &primitive = ecs_coordinator_->getComponent<Primitive>( entity );
      // Set the begin and end from our primitive component
      const f32 vertices[] = { primitive.begin.x, primitive.begin.y, primitive.end.x,
                               primitive.end.y };
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
