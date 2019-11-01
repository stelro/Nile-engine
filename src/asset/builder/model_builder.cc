#include "Nile/asset/builder/model_builder.hh"
#include "Nile/core/assert.hh"


namespace nile::AssetBuilder {

  static TextureType assimpTypeToTextureType( aiTextureType type ) noexcept {
    switch ( type ) {
      case aiTextureType_DIFFUSE:
        return TextureType::DIFFUSE;
      case aiTextureType_SPECULAR:
        return TextureType::SPECULAR;
      case aiTextureType_AMBIENT:
        return TextureType::AMBIENT;
      case aiTextureType_HEIGHT:
        return TextureType::NORMAL;
      case aiTextureType_EMISSIVE:
        return TextureType::EMISSIVE;
    }
  }

  Builder<Model>::Builder( const std::shared_ptr<nile::AssetManager> &assetManager ) noexcept
      : m_assetManager( assetManager ) {}

  void Builder<Model>::loadModel() noexcept {
    Assimp::Importer import;
    const aiScene *scene = import.ReadFile( m_path, aiProcess_Triangulate | aiProcess_FlipUVs );

    if ( !scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode ) {
      log::error( "Assimp: %s\n", import.GetErrorString() );
      return;
    }

    m_directoryName = m_path.substr( 0, m_path.find_last_of( '/' ) );
    processNode( scene->mRootNode, scene );
  }

  void Builder<Model>::processNode( aiNode *node, const aiScene *scene ) noexcept {

    for ( u32 i = 0; i < node->mNumMeshes; i++ ) {
      aiMesh *mesh = scene->mMeshes[ node->mMeshes[ i ] ];
      m_meshes.push_back( processMesh( mesh, scene ) );
    }

    for ( u32 i = 0; i < node->mNumChildren; i++ ) {
      processNode( node->mChildren[ i ], scene );
    }
  }

  Mesh Builder<Model>::processMesh( aiMesh *mesh, const aiScene *scene ) noexcept {

    std::vector<Vertex> vertices;
    std::vector<u32> indices;
    std::vector<Texture2D *> textures;

    log::print( "%s\n", mesh->mName.C_Str() );

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
        vertex.uv.x = mesh->mTextureCoords[ 0 ][ i ].x;
        vertex.uv.y = mesh->mTextureCoords[ 0 ][ i ].y;
      } else {
        vertex.uv = glm::vec2( 0.0f, 0.0f );
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
      std::vector<Texture2D *> diffuseMaps =
          loadMaterialTextures( material, aiTextureType_DIFFUSE );

      textures.insert( textures.end(), diffuseMaps.begin(), diffuseMaps.end() );

      // specular maps
      std::vector<Texture2D *> specularMaps =
          loadMaterialTextures( material, aiTextureType_SPECULAR );
      textures.insert( textures.end(), specularMaps.begin(), specularMaps.end() );

      // normal maps
      std::vector<Texture2D *> normalMaps = loadMaterialTextures( material, aiTextureType_HEIGHT );
      textures.insert( textures.end(), normalMaps.begin(), normalMaps.end() );

      // ambient maps
      std::vector<Texture2D *> heightMaps = loadMaterialTextures( material, aiTextureType_AMBIENT );
      textures.insert( textures.end(), normalMaps.begin(), normalMaps.end() );
    }

    return Mesh( vertices, indices, textures );
  }

  std::vector<Texture2D *> Builder<Model>::loadMaterialTextures( aiMaterial *material,
                                                                 aiTextureType type ) noexcept {

    std::vector<Texture2D *> textures;
    for ( u32 i = 0; i < material->GetTextureCount( type ); i++ ) {
      aiString str;
      material->GetTexture( type, i, &str );
      std::string filename = m_directoryName + '/' + str.C_Str();
      auto texture = m_assetManager->loadAsset<Texture2D>( str.C_Str(), filename );
      log::print( "texture name: %s\n", str.C_Str() );
      texture->setTexturetype( assimpTypeToTextureType( type ) );
      textures.push_back( texture );
    }

    return textures;
  }

  Builder<Model> &Builder<Model>::setModelPath( std::string_view path ) noexcept {
    m_path = path;
    return *this;
  }

  [[nodiscard]] Model *Builder<Model>::build() noexcept {
    ASSERT_M( !m_path.empty(), "Model path is empty!" );
    this->loadModel();
    Model *model = new Model( m_meshes );
    return model;
  }

}    // namespace nile::AssetBuilder
