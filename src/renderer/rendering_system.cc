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

namespace nile {

  RenderingSystem::RenderingSystem( const std::shared_ptr<Coordinator> &coordinator,
                                    const std::shared_ptr<ShaderSet> &shader ) noexcept
      : m_ecsCoordinator( coordinator )
      , m_shader( shader ) {}

  void RenderingSystem::create() noexcept {
    this->initRenderData();
  }

  void RenderingSystem::update( float dt ) noexcept {}

  void RenderingSystem::render( float dt ) noexcept {

    glEnable( GL_CULL_FACE );

    for ( const auto &entity : m_entities ) {


      auto &mesh = m_ecsCoordinator->getComponent<MeshComponent>( entity );
      auto &transform = m_ecsCoordinator->getComponent<Transform>( entity );
      auto &renderable = m_ecsCoordinator->getComponent<Renderable>( entity );

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

        m_shader->SetInteger( ( "texture_" + TextureTypeStr( type ) + number ).c_str(), i );
        mesh.textures[ i ]->bind();
      }

      this->m_shader->use();

      glm::mat4 model = glm::mat4 {1.0f};
      model = glm::translate( model, transform.position );
      model =
          glm::rotate( model, glm::radians( transform.xRotation ), glm::vec3( 1.0f, 0.0f, 0.0f ) );
      model =
          glm::rotate( model, glm::radians( transform.yRotation ), glm::vec3( 0.0f, 1.0f, 0.0f ) );
      model =
          glm::rotate( model, glm::radians( transform.zRotation ), glm::vec3( 0.0f, 0.0f, 1.0f ) );

      model = glm::scale( model, transform.scale );

      m_shader->SetMatrix4( "model", model );
      m_shader->SetVector3f( "color", renderable.color );

      glActiveTexture( GL_TEXTURE0 );

      glBindVertexArray( mesh.vao );
      glDrawElements( GL_TRIANGLES, mesh.indices.size(), GL_UNSIGNED_INT, 0 );
      glBindVertexArray( 0 );
      glBindTexture( GL_TEXTURE_2D, 0 );
    }

    glDisable( GL_CULL_FACE );
  }

  void RenderingSystem::initRenderData() noexcept {


    for ( const auto &entity : m_entities ) {

      auto &mesh = m_ecsCoordinator->getComponent<MeshComponent>( entity );

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
