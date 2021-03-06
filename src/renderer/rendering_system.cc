#include "Nile/renderer/rendering_system.hh"
#include "Nile/ecs/components/mesh_component.hh"
#include "Nile/ecs/components/renderable.hh"
#include "Nile/ecs/components/transform.hh"
#include "Nile/ecs/ecs_coordinator.hh"
#include "Nile/log/log.hh"
#include "Nile/renderer/shaderset.hh"
#include "Nile/renderer/texture2d.hh"
#include "Nile/utils/vertex.hh"

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <spdlog/spdlog.h>

namespace nile {

  RenderingSystem::RenderingSystem( const std::shared_ptr<Coordinator> &coordinator,
                                    const std::shared_ptr<ShaderSet> &shader ) noexcept
      : ecs_coordinator_( coordinator )
      , shader_( shader ) {}

  void RenderingSystem::create() noexcept {
    this->init_rendering_data();
    spdlog::info(
        "ECS RenderingSystem has been registered to ECS manager and created successfully." );
  }

  void RenderingSystem::update( float dt ) noexcept {}

  void RenderingSystem::render( float dt ) noexcept {

    glEnable( GL_CULL_FACE );

    for ( const auto &entity : entities_ ) {

      auto &mesh = ecs_coordinator_->getComponent<MeshComponent>( entity );
      auto &transform = ecs_coordinator_->getComponent<Transform>( entity );
      auto &renderable = ecs_coordinator_->getComponent<Renderable>( entity );

      // Allow user to set user-defined shaders
      // if the shaderset is set in the renderable component, then use it
      // otherwise use the default one provided by the rendering system
      auto shader = ( renderable.shaderSet ) ? renderable.shaderSet : shader_;

      if ( renderable.blend )
        glEnable( GL_BLEND );
      else
        glDisable( GL_BLEND );

      u32 diffuse_nr = 1;
      u32 specular_nr = 1;
      u32 normal_nr = 1;

      for ( u32 i = 0; i < mesh.textures.size(); i++ ) {

        glActiveTexture( GL_TEXTURE0 + i );

        // @Fixme(stel): replace strings with string_view
        std::string number;
        auto type = mesh.textures[ i ]->getTextureType();

        if ( type == TextureType::DIFFUSE )
          number = std::to_string( diffuse_nr++ );
        else if ( type == TextureType::SPECULAR )
          number = std::to_string( specular_nr++ );
        else if ( type == TextureType::NORMAL )
          number = std::to_string( normal_nr++ );

        shader->SetInteger( ( "material." + TextureTypeStr( type ) + number ).c_str(), i );
        mesh.textures[ i ]->bind();
      }

      shader->use();

      glm::mat4 model = glm::mat4 { 1.0f };
      model = glm::translate( model, transform.position );
      model =
          glm::rotate( model, glm::radians( transform.xRotation ), glm::vec3( 1.0f, 0.0f, 0.0f ) );
      model =
          glm::rotate( model, glm::radians( transform.yRotation ), glm::vec3( 0.0f, 1.0f, 0.0f ) );
      model =
          glm::rotate( model, glm::radians( transform.zRotation ), glm::vec3( 0.0f, 0.0f, 1.0f ) );

      model = glm::scale( model, transform.scale );

      shader->SetMatrix4( "model", model );
      shader->SetVector3f( "objectColor", renderable.color );

      glActiveTexture( GL_TEXTURE0 );

      glBindVertexArray( mesh.vao );
      glDrawElements( GL_TRIANGLES, mesh.indices.size(), GL_UNSIGNED_INT, 0 );
      glBindVertexArray( 0 );
      glBindTexture( GL_TEXTURE_2D, 0 );

      if ( renderable.blend )
        glDisable( GL_BLEND );
      else
        glEnable( GL_BLEND );
    }

    glDisable( GL_CULL_FACE );
  }

  void RenderingSystem::init_rendering_data() noexcept {

    for ( const auto &entity : entities_ ) {

      auto &mesh = ecs_coordinator_->getComponent<MeshComponent>( entity );

      glGenVertexArrays( 1, &mesh.vao );
      glGenBuffers( 1, &mesh.ebo );
      glGenBuffers( 1, &mesh.vbo );

      glBindVertexArray( mesh.vao );

      // Vertex buffer object
      glBindBuffer( GL_ARRAY_BUFFER, mesh.vbo );

      glBufferData( GL_ARRAY_BUFFER, mesh.vertices.size() * sizeof( Vertex ), &mesh.vertices[ 0 ],
                    GL_STATIC_DRAW );

      // Element buffer object
      glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, mesh.ebo );
      glBufferData( GL_ELEMENT_ARRAY_BUFFER, mesh.indices.size() * sizeof( u32 ),
                    &mesh.indices[ 0 ], GL_STATIC_DRAW );

      // Vertex positions
      glEnableVertexAttribArray( 0 );
      glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, sizeof( Vertex ), ( void * )0 );

      // Vertex Normals
      glEnableVertexAttribArray( 1 );
      glVertexAttribPointer( 1, 3, GL_FLOAT, GL_FALSE, sizeof( Vertex ),
                             ( void * )offsetof( Vertex, normal ) );

      // UV coordinates
      glEnableVertexAttribArray( 2 );
      glVertexAttribPointer( 2, 2, GL_FLOAT, GL_FALSE, sizeof( Vertex ),
                             ( void * )offsetof( Vertex, uv ) );

      // Unbind
      glBindBuffer( GL_ARRAY_BUFFER, 0 );
      glBindVertexArray( 0 );
      glCheckError();
    }
  }

}    // namespace nile
