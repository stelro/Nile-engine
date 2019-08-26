#include "resource/resource_manager.hh"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "core/nile.hh"
#include "renderer/shader.hh"
#include "renderer/texture2d.hh"

#include <fstream>
#include <iostream>
#include <sstream>

#include <GL/glew.h>

namespace nile {

  ResourceManager *ResourceManager::m_instance = nullptr;

  std::map<std::string, std::shared_ptr<Shader>> ResourceManager::shaders;
  std::map<std::string, std::shared_ptr<Texture2D>> ResourceManager::textures;

  ResourceManager::ResourceManager() noexcept {

  }

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
    std::ifstream fragmentShaderFile( fshaderFile.data() );
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
      std::stringstream gShaderStream;
      gShaderStream << geometryShaderFile.rdbuf();
      geometryShaderFile.close();
      geometryCode = gShaderStream.str();
    }

    // TODO @stel: Error checking

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
    unsigned char *image = stbi_load( file.data(), &width, &height, &nrChannels, 0 );
    texture.generate( width, height, image );
    stbi_image_free( image );
    return texture;
  }


}    // namespace nile
