#include "Nile/renderer/shader.hh"


#include "Nile/core/logger.hh"
#include <GL/glew.h>
#include <glm/gtc/type_ptr.hpp>

namespace nile {

  Shader &Shader::use() noexcept {
    glUseProgram( this->m_id );
    return *this;
  }

  Shader::~Shader() noexcept {}

  void Shader::checkCompileErrors( u32 object, std::string_view type ) noexcept {
    GLint success;
    char infoLog[ 1024 ];
    if ( type != "PROGRAM" ) {
      glGetShaderiv( object, GL_COMPILE_STATUS, &success );
      if ( !success ) {
        glGetShaderInfoLog( object, 1024, NULL, infoLog );
        log::error( "::SHADER: Compile-time error: Type %s\n%s\n", type, infoLog );
      }
    } else {
      glGetProgramiv( object, GL_LINK_STATUS, &success );
      if ( !success ) {
        glGetProgramInfoLog( object, 1024, NULL, infoLog );
        log::error( "::SHADER: Link-time error: Type: %s\n%s\n", type, infoLog );
      }
    }
  }

  void Shader::compile( const char *vertexSource, const char *fragmentSource,
                        const char *geometrySource ) noexcept {
    GLuint sVertex, sFragment, gShader;
    // Vertex Shader
    sVertex = glCreateShader( GL_VERTEX_SHADER );
    glShaderSource( sVertex, 1, &vertexSource, NULL );
    glCompileShader( sVertex );
    checkCompileErrors( sVertex, "VERTEX" );
    // Fragment Shader
    sFragment = glCreateShader( GL_FRAGMENT_SHADER );
    glShaderSource( sFragment, 1, &fragmentSource, NULL );
    glCompileShader( sFragment );
    checkCompileErrors( sFragment, "FRAGMENT" );
    // If geometry shader source code is given, also compile geometry shader
    if ( geometrySource != nullptr ) {
      gShader = glCreateShader( GL_GEOMETRY_SHADER );
      glShaderSource( gShader, 1, &geometrySource, NULL );
      glCompileShader( gShader );
      checkCompileErrors( gShader, "GEOMETRY" );
    }
    // Shader Program
    this->m_id = glCreateProgram();
    glAttachShader( this->m_id, sVertex );
    glAttachShader( this->m_id, sFragment );
    if ( geometrySource != nullptr )
      glAttachShader( this->m_id, gShader );
    glLinkProgram( this->m_id );
    checkCompileErrors( this->m_id, "PROGRAM" );
    // Delete the shaders as they're linked into our program now and no longer necessery
    glDeleteShader( sVertex );
    glDeleteShader( sFragment );
    if ( geometrySource != nullptr )
      glDeleteShader( gShader );
  }


  void Shader::SetFloat( const char *name, f32 value, bool useShader ) noexcept {
    if ( useShader )
      this->use();
    glUniform1f( glGetUniformLocation( this->m_id, name ), value );
  }
  void Shader::SetInteger( const char *name, i32 value, bool useShader ) noexcept {
    if ( useShader )
      this->use();
    glUniform1i( glGetUniformLocation( this->m_id, name ), value );
  }
  void Shader::SetVector2f( const char *name, f32 x, f32 y, bool useShader ) noexcept {
    if ( useShader )
      this->use();
    glUniform2f( glGetUniformLocation( this->m_id, name ), x, y );
  }
  void Shader::SetVector2f( const char *name, const glm::vec2 &value, bool useShader ) noexcept {
    if ( useShader )
      this->use();
    glUniform2f( glGetUniformLocation( this->m_id, name ), value.x, value.y );
  }
  void Shader::SetVector3f( const char *name, f32 x, f32 y, f32 z, bool useShader ) noexcept {
    if ( useShader )
      this->use();
    glUniform3f( glGetUniformLocation( this->m_id, name ), x, y, z );
  }
  void Shader::SetVector3f( const char *name, const glm::vec3 &value, bool useShader ) noexcept {
    if ( useShader )
      this->use();
    glUniform3f( glGetUniformLocation( this->m_id, name ), value.x, value.y, value.z );
  }
  void Shader::SetVector4f( const char *name, f32 x, f32 y, f32 z, f32 w,
                            bool useShader ) noexcept {
    if ( useShader )
      this->use();
    glUniform4f( glGetUniformLocation( this->m_id, name ), x, y, z, w );
  }
  void Shader::SetVector4f( const char *name, const glm::vec4 &value, bool useShader ) noexcept {
    if ( useShader )
      this->use();
    glUniform4f( glGetUniformLocation( this->m_id, name ), value.x, value.y, value.z, value.w );
  }
  void Shader::SetMatrix4( const char *name, const glm::mat4 &matrix, bool useShader ) noexcept {
    if ( useShader )
      this->use();
    glUniformMatrix4fv( glGetUniformLocation( this->m_id, name ), 1, GL_FALSE,
                        glm::value_ptr( matrix ) );
  }

}    // namespace nile
