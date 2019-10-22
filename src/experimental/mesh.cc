#include "Nile/experimental/mesh.hh"

#include <GL/glew.h>

namespace nile::experimental {

  Mesh::Mesh( std::vector<Vertex> t_vertices, std::vector<u32> t_indices,
              std::vector<Texture> t_textures ) noexcept
      : vertices( std::move( t_vertices ) )
      , indices( std::move( t_indices ) )
      , textures( std::move( t_textures ) ) {
    this->setupMesh();
  }


  void Mesh::draw( ShaderSet *shader ) {

    u32 diffuseNr = 1;
    u32 specularNr = 1;

    for ( u32 i = 0; i < textures.size(); i++ ) {

      glActiveTexture( GL_TEXTURE0 + i );

      std::string number;
      std::string name = textures[ i ].type;

      if ( name == "texture_diffuse" )
        number = std::to_string( diffuseNr++ );
      else if ( name == "texture_specular" )
        number = std::to_string( specularNr++ );

      shader->SetFloat( ( "material." + name + number ).c_str(), i );
      glBindTexture( GL_TEXTURE_2D, textures[ i ].id );
    }

    glActiveTexture( GL_TEXTURE0 );

    // draw mesh
    glBindVertexArray( m_vao );
    glDrawElements( GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0 );
    glBindVertexArray( 0 );
  }

  void Mesh::setupMesh() noexcept {

    glGenVertexArrays( 1, &this->m_vao );
    glGenBuffers( 1, &this->m_vbo );
    glGenBuffers( 1, &this->m_ebo );

    glBindVertexArray( this->m_vao );

    // vbo
    glBindBuffer( GL_ARRAY_BUFFER, this->m_vbo );
    glBufferData( GL_ARRAY_BUFFER, vertices.size() * sizeof( Vertex ), &vertices[ 0 ],
                  GL_STATIC_DRAW );
    // ebo
    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, this->m_ebo );
    glBufferData( GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof( u32 ), &indices[ 0 ],
                  GL_STATIC_DRAW );

    // vertex positions
    glEnableVertexAttribArray( 0 );
    glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, sizeof( Vertex ), ( void * )0 );

    // vertex normals
    glEnableVertexAttribArray( 1 );
    glVertexAttribPointer( 1, 3, GL_FLOAT, GL_FALSE, sizeof( Vertex ),
                           ( void * )offsetof( Vertex, normal ) );

    // vertex tex coords
    glEnableVertexAttribArray( 2 );
    glVertexAttribPointer( 2, 2, GL_FLOAT, GL_FALSE, sizeof( Vertex ),
                           ( void * )offsetof( Vertex, texCoords ) );

    // Unbind
    glBindVertexArray( 0 );
  }


}    // namespace nile::experimental
