/* ================================================================================
$File: shaderset.cc
$Date: $
$Revision: $
$Creator: Rostislav Orestis Stelmach
$Notice: $
================================================================================ */

#include "Nile/renderer/shaderset.hh"

#include <GL/glew.h>
#include <glm/gtc/type_ptr.hpp>

namespace nile {

  ShaderSet::ShaderSet( u32 id ) noexcept
      : m_programId( id ) {}

  ShaderSet &ShaderSet::use() noexcept {
    glUseProgram( this->m_programId );
    return *this;
  }

  void ShaderSet::SetFloat( const char *name, f32 value, bool useShader ) noexcept {
    if ( useShader )
      this->use();
    glUniform1f( glGetUniformLocation( m_programId, name ), value );
  }

  void ShaderSet::SetInteger( const char *name, i32 value, bool useShader ) noexcept {
    if ( useShader )
      this->use();
    glUniform1i( glGetUniformLocation( m_programId, name ), value );
  }

  void ShaderSet::SetVector2f( const char *name, f32 x, f32 y, bool useShader ) noexcept {
    if ( useShader )
      this->use();
    glUniform2f( glGetUniformLocation( m_programId, name ), x, y );
  }

  void ShaderSet::SetVector2f( const char *name, const glm::vec2 &value, bool useShader ) noexcept {
    if ( useShader )
      this->use();
    glUniform2f( glGetUniformLocation( m_programId, name ), value.x, value.y );
  }

  void ShaderSet::SetVector3f( const char *name, f32 x, f32 y, f32 z, bool useShader ) noexcept {
    if ( useShader )
      this->use();
    glUniform3f( glGetUniformLocation( m_programId, name ), x, y, z );
  }

  void ShaderSet::SetVector3f( const char *name, const glm::vec3 &value, bool useShader ) noexcept {
    if ( useShader )
      this->use();
    glUniform3f( glGetUniformLocation( m_programId, name ), value.x, value.y, value.z );
  }

  void ShaderSet::SetVector4f( const char *name, f32 x, f32 y, f32 z, f32 w,
                               bool useShader ) noexcept {
    if ( useShader )
      this->use();
    glUniform4f( glGetUniformLocation( m_programId, name ), x, y, z, w );
  }

  void ShaderSet::SetVector4f( const char *name, const glm::vec4 &value, bool useShader ) noexcept {
    if ( useShader )
      this->use();
    glUniform4f( glGetUniformLocation( m_programId, name ), value.x, value.y, value.z, value.w );
  }

  void ShaderSet::SetMatrix4( const char *name, const glm::mat4 &matrix, bool useShader ) noexcept {
    if ( useShader )
      this->use();
    glUniformMatrix4fv( glGetUniformLocation( m_programId, name ), 1, GL_FALSE,
                        glm::value_ptr( matrix ) );
  }

}    // namespace nile
