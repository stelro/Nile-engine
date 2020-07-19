/* ================================================================================
$File: shaderset_builder.cc
$Date: $
$Revision: $
$Creator: Rostislav Orestis Stelmach
$Notice: $
================================================================================ */

#include "Nile/asset/builder/shaderset_builder.hh"
#include "Nile/core/assert.hh"
#include "Nile/core/file_system.hh"

#include <GL/glew.h>
#include <spdlog/spdlog.h>

#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>

namespace nile::AssetBuilder {

  Builder<ShaderSet> &Builder<ShaderSet>::setVertexPath( const std::string &value ) noexcept {
    m_vertexPath = value;
    return *this;
  }

  Builder<ShaderSet> &Builder<ShaderSet>::setFragmentPath( const std::string &value ) noexcept {
    m_fragmentPath = value;
    return *this;
  }

  Builder<ShaderSet> &Builder<ShaderSet>::setGeometryPath( const std::string &value ) noexcept {
    m_geometryPath = value;
    return *this;
  }

  [[nodiscard]] std::shared_ptr<ShaderSet> Builder<ShaderSet>::build() noexcept {

    auto shaderset = std::make_shared<ShaderSet>(
        loadShaderFromFile( m_vertexPath, m_fragmentPath, m_geometryPath ), m_vertexPath,
        m_fragmentPath, m_geometryPath );

#if defined( TOOLS_ENABLED )

    // @optimization: maybe we could do this better?

    namespace fs = std::filesystem;

    FileSize file_size_struct;
    TimeStamp time_stamp_struct;

    fs::path frag_path = m_fragmentPath;
    fs::path vertex_path = m_vertexPath;
    fs::path geom_path = m_geometryPath;

    if ( fs::exists( frag_path ) ) {
      file_size_struct.fragment = fs::file_size( frag_path );
      time_stamp_struct.fragment = fs::last_write_time( frag_path );
    }

    if ( fs::exists( vertex_path ) ) {
      file_size_struct.vertex = fs::file_size( vertex_path );
      time_stamp_struct.vertex = fs::last_write_time( vertex_path );
    }

    if ( fs::exists( geom_path ) ) {
      file_size_struct.geometry = fs::file_size( geom_path );
      time_stamp_struct.geometry = fs::last_write_time( geom_path );
    }

    shaderset->setTimeStamp( time_stamp_struct );
    shaderset->setFileSize( file_size_struct );

#endif

    return shaderset;
  }

  u32 Builder<ShaderSet>::loadShaderFromFile( std::string_view vshaderFile,
                                              std::string_view fshaderFile,
                                              std::string_view gshaderFile ) noexcept {


    u32 program_id = 0;

    // 1. Retrieve the vertex/fragment source code from filePath
    std::string vertexCode;
    std::string fragmentCode;
    std::string geometryCode;

    // Open files
    std::ifstream vertexShaderFile( vshaderFile.data() );
    if ( !vertexShaderFile.is_open() ) {
      spdlog::error( "Failed to open \"{}\" file for vertex shader", vshaderFile.data() );
    } else {
      spdlog::debug( "Shader \"{}\" has been loaded.", vshaderFile.data() );
    }

    std::ifstream fragmentShaderFile( fshaderFile.data() );
    if ( !fragmentShaderFile.is_open() ) {
      spdlog::error( "Failed to open \"{}\" file for fragment shader", fshaderFile.data() );
    } else {
      spdlog::debug( "Shader \"{}\" has been loaded.", fshaderFile.data() );
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
        spdlog::error( "Failed to open \"{}\" file for geometry shader", gshaderFile.data() );
      } else {
        spdlog::debug( "Shader \"{}\" has been loaded", gshaderFile.data() );
      }

      std::stringstream gShaderStream;
      gShaderStream << geometryShaderFile.rdbuf();
      geometryShaderFile.close();
      geometryCode = gShaderStream.str();
    }

    const GLchar *vShaderCode = vertexCode.c_str();
    const GLchar *fShaderCode = fragmentCode.c_str();
    const GLchar *gShaderCode = geometryCode.c_str();

    program_id =
        compileShaders( vShaderCode, fShaderCode, gshaderFile.size() != 0 ? gShaderCode : nullptr );

    return program_id;
  }

  u32 Builder<ShaderSet>::compileShaders( const char *vSource, const char *fSource,
                                          const char *gSource ) noexcept {

    GLuint sVertex, sFragment, gShader;
    // Vertex Shader
    sVertex = glCreateShader( GL_VERTEX_SHADER );
    glShaderSource( sVertex, 1, &vSource, nullptr );
    glCompileShader( sVertex );
    checkForCompileErrors( sVertex, ShaderType::VERTEX );
    // Fragment Shader
    sFragment = glCreateShader( GL_FRAGMENT_SHADER );
    glShaderSource( sFragment, 1, &fSource, nullptr );
    glCompileShader( sFragment );
    checkForCompileErrors( sFragment, ShaderType::FRAGMENT );

    // If geometry shader source code is given, also compile geometry shader
    if ( gSource != nullptr ) {
      gShader = glCreateShader( GL_GEOMETRY_SHADER );
      glShaderSource( gShader, 1, &gSource, nullptr );
      glCompileShader( gShader );
      checkForCompileErrors( gShader, ShaderType::GEOMETRY );
    }

    // Shader Program
    u32 program_id = glCreateProgram();
    glAttachShader( program_id, sVertex );
    glAttachShader( program_id, sFragment );

    if ( gSource != nullptr )
      glAttachShader( program_id, gShader );
    glLinkProgram( program_id );

    checkForCompileErrors( program_id, ShaderType::PROGRAM );
    // Delete the shaders as they're linked into our program now and no longer necessery
    glDeleteShader( sVertex );
    glDeleteShader( sFragment );

    if ( gSource != nullptr )
      glDeleteShader( gShader );

    return program_id;
  }

  void Builder<ShaderSet>::checkForCompileErrors( u32 object, ShaderType type ) noexcept {

    GLint success;
    char infoLog[ 1024 ];
    if ( type != ShaderType::PROGRAM ) {
      glGetShaderiv( object, GL_COMPILE_STATUS, &success );
      if ( !success ) {
        glGetShaderInfoLog( object, 1024, NULL, infoLog );
        spdlog::error( "::SHADER: Compile-time error: Type {1}\n{2}\n", type, infoLog );
      }
    } else {
      glGetProgramiv( object, GL_LINK_STATUS, &success );
      if ( !success ) {
        glGetProgramInfoLog( object, 1024, NULL, infoLog );
        spdlog::error( "::SHADER: Link-time error: Type: {1}\n{2}\n", type, infoLog );
      }
    }
  }


}    // namespace nile::AssetBuilder
