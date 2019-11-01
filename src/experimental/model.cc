#include "Nile/experimental/model.hh"
#include "Nile/core/logger.hh"
#include "Nile/renderer/texture2d.hh"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace nile::experimental {

  void Model::loadModel( std::string path ) {
    Assimp::Importer import;
    const aiScene *scene = import.ReadFile( path, aiProcess_Triangulate | aiProcess_FlipUVs );

    if ( !scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode ) {
      log::error( "Assimp: %s\n", import.GetErrorString() );
      return;
    }

    directory = path.substr( 0, path.find_last_of( '/' ) );
    processNode( scene->mRootNode, scene );
  }

  void Model::processNode( aiNode *node, const aiScene *scene ) {

    for ( u32 i = 0; i < node->mNumMeshes; i++ ) {
      aiMesh *mesh = scene->mMeshes[ node->mMeshes[ i ] ];
      meshes.push_back( processMesh( mesh, scene ) );
    }

    for ( u32 i = 0; i < node->mNumChildren; i++ ) {
      processNode( node->mChildren[ i ], scene );
    }
  }

  Mesh Model::processMesh( aiMesh *mesh, const aiScene *scene ) {

    std::vector<Vertex> vertices;
    std::vector<u32> indices;
    std::vector<Texture> textures;
    
    log::print("%s\n", mesh->mName.C_Str());

    for ( u32 i = 0; i < mesh->mNumVertices; i++ ) {
      Vertex vertex;

      // Retrive positions
      vertex.position.x = mesh->mVertices[ i ].x;
      vertex.position.y = mesh->mVertices[ i ].y;
      vertex.position.z = mesh->mVertices[ i ].z;

      // Retrive Normals
      vertex.normal.x = mesh->mNormals[ i ].x;
      vertex.normal.y = mesh->mNormals[ i ].y;
      vertex.normal.z = mesh->mNormals[ i ].z;

      if ( mesh->mTextureCoords[ 0 ] ) {
        vertex.texCoords.x = mesh->mTextureCoords[ 0 ][ i ].x;
        vertex.texCoords.y = mesh->mTextureCoords[ 0 ][ i ].y;
      } else {
        vertex.texCoords = glm::vec2( 0.0f, 0.0f );
      }

      vertices.push_back( vertex );
    }

    for ( u32 i = 0; i < mesh->mNumFaces; i++ ) {
      aiFace face = mesh->mFaces[ i ];
      for ( u32 j = 0; j < face.mNumIndices; j++ ) {
        indices.push_back( face.mIndices[ j ] );
      }
    }

    if ( mesh->mMaterialIndex >= 0 ) {

      aiMaterial *material = scene->mMaterials[ mesh->mMaterialIndex ];

      // diffuse maps
      std::vector<Texture> diffuseMaps =
          loadMaterialTextures( material, aiTextureType_DIFFUSE, "texture_diffuse" );

      textures.insert( textures.end(), diffuseMaps.begin(), diffuseMaps.end() );

      // specular maps
      std::vector<Texture> specularMaps =
          loadMaterialTextures( material, aiTextureType_SPECULAR, "texture_specular" );
      textures.insert( textures.end(), specularMaps.begin(), specularMaps.end() );

      // normal maps
      std::vector<Texture> normalMaps =
          loadMaterialTextures( material, aiTextureType_HEIGHT, "texture_normal" );
      textures.insert( textures.end(), normalMaps.begin(), normalMaps.end() );

      // height maps
      std::vector<Texture> heightMaps =
          loadMaterialTextures( material, aiTextureType_AMBIENT, "texture_height" );
      textures.insert( textures.end(), normalMaps.begin(), normalMaps.end() );
    }

    return Mesh( vertices, indices, textures );
  }

  std::vector<Texture> Model::loadMaterialTextures( aiMaterial *mat, aiTextureType type,
                                                    std::string typeName ) {

    std::vector<Texture> textures;
    for ( u32 i = 0; i < mat->GetTextureCount( type ); i++ ) {
      aiString str;
      mat->GetTexture( type, i, &str );
      Texture texture;
      std::string filename = directory + '/' + str.C_Str();
      texture.id = m_assetManager->loadAsset<Texture2D>( str.C_Str(), filename )->getID();
      log::print("texture name: %s\n", str.C_Str());
      log::print("texture type: %s\n", typeName.c_str());
      texture.type = typeName;
      textures.push_back( texture );
    }

    return textures;
  }

  void Model::draw( ShaderSet *shader ) {

    glm::mat4 model = glm::mat4( 1.0f );
    model = glm::translate( model, glm::vec3( 0.0f, 0.0f, -11.0f ) );
    model = glm::scale( model, glm::vec3( 0.2f, 0.2f, 0.2f ) );
    shader->SetMatrix4( "model", model );


    for ( auto &i : meshes )
      i.draw( shader );
  }


}    // namespace nile::experimental
