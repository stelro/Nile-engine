#include "Nile/resource/resource_manager.hh"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "Nile/core/logger.hh"
#include "Nile/core/nile.hh"
#include "Nile/renderer/shader.hh"
#include "Nile/renderer/texture2d.hh"

#include <fstream>
#include <iostream>
#include <sstream>

#include <GL/glew.h>

namespace nile {

  ResourceManager *ResourceManager::m_instance = nullptr;

  std::map<std::string, std::shared_ptr<Shader>> ResourceManager::shaders;
  std::map<std::string, std::shared_ptr<Texture2D>> ResourceManager::textures;

  ResourceManager::ResourceManager() noexcept {}

  ResourceManager *ResourceManager::getInstance() noexcept {

    if ( !m_instance )
      m_instance = new ResourceManager();

    return m_instance;
  }

  void ResourceManager::destroy() noexcept {
    ASSERT_M( m_instance, "Tried to delete un-initialized singleton class, ResourceManager" );
    delete m_instance;
    m_instance = nullptr;
  }

  Shader ResourceManager::loadShaderFromFile( std::string_view vshaderFile,
                                              std::string_view fshaderFile,
                                              std::string_view gshaderFile ) noexcept {

    // 1. Retrieve the vertex/fragment source code from filePath
    std::string vertexCode;
    std::string fragmentCode;
    std::string geometryCode;


    // Open files
    std::ifstream vertexShaderFile( vshaderFile.data() );
    if ( !vertexShaderFile.is_open() ) {
      log::error( "Failed to open { %s } file for vertex shader\n", vshaderFile.data() );
    }

    std::ifstream fragmentShaderFile( fshaderFile.data() );
    if ( !fragmentShaderFile.is_open() ) {
      log::error( "Failed to open { %s } file for fragment shader\n", fshaderFile.data() );
    }

    std::stringstream vShaderStream, fShaderStream;
    // Read file's buffer contents into streams
    vShaderStream << vertexShaderFile.rdbuf();
    fShaderStream << fragmentShaderFile.rdbuf();

    // close file handlers
    vertexShaderFile.close();
    fragmentShaderFile.close();

    // Convert stream into string
    vertexCode = vShaderStream.str();
    fragmentCode = fShaderStream.str();

    // If geometry shader path is present, also load a geometry shader
    if ( gshaderFile.size() != 0 ) {
      std::ifstream geometryShaderFile( gshaderFile.data() );
      if ( !geometryShaderFile.is_open() ) {
        log::error( "Failed to open { %s } file for geometry shader", gshaderFile.data() );
      }

      std::stringstream gShaderStream;
      gShaderStream << geometryShaderFile.rdbuf();
      geometryShaderFile.close();
      geometryCode = gShaderStream.str();
    }


    const GLchar *vShaderCode = vertexCode.c_str();
    const GLchar *fShaderCode = fragmentCode.c_str();
    const GLchar *gShaderCode = geometryCode.c_str();
    // 2. Now create shader object from source code
    Shader shader;
    shader.compile( vShaderCode, fShaderCode, gshaderFile.size() != 0 ? gShaderCode : nullptr );
    return shader;
  }

  Texture2D ResourceManager::loadTextureFromFile( std::string_view file, bool alpha ) noexcept {

    Texture2D texture;
    if ( alpha ) {
      texture.setInternalFormat( GL_RGBA );
      texture.setImageFormat( GL_RGBA );
    }

    // Load image
    int width, height, nrChannels;
    unsigned char *image = nullptr;
    image = stbi_load( file.data(), &width, &height, &nrChannels, STBI_rgb_alpha );

    if ( !image ) {
      log::error( "Failed to load texture { %s }\n", file.data() );
    }

    texture.generate( width, height, image );
    stbi_image_free( image );
    return texture;
  }

  void ResourceManager::clear() noexcept {}

  std::shared_ptr<Shader> ResourceManager::loadShader( std::string_view vshaderFile,
                                                       std::string_view fshaderFile,
                                                       std::string_view gshaderFile,
                                                       const std::string &name ) noexcept {
    shaders[ name ] =
        std::make_shared<Shader>( loadShaderFromFile( vshaderFile, fshaderFile, gshaderFile ) );

    return shaders[ name ];
  }

  std::shared_ptr<Texture2D> ResourceManager::loadTexture( std::string_view file, bool alpha,
                                                           const std::string &name ) noexcept {
    textures[ name ] = std::make_shared<Texture2D>( loadTextureFromFile( file, alpha ) );
    return textures[ name ];
  }

  std::shared_ptr<Shader> ResourceManager::getShader( const std::string &name ) noexcept {
    // TODO @stel: check for shader if exist before return
    auto it = shaders.find( name );
    if ( it != shaders.end() ) {
      return it->second;
    } else {
      log::error( "Couldn't find any shader with the id :  { %s }\n", name.c_str() );
      return nullptr;
    }
  }

  std::shared_ptr<Texture2D> ResourceManager::getTexture( const std::string &name ) noexcept {
    // TODO @stel: check for texture if exist before return
    auto it = textures.find( name );
    if ( it != textures.end() ) {
      return it->second;
    } else {
      log::error( "Couldn't find any texture with the id { %s }\n", name.c_str() );
      return nullptr;
    }
  }


}    // namespace nile
