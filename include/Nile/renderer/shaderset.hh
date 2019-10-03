/* ================================================================================
$File: shaderset.hh
$Date: $
$Revision: $
$Creator: Rostislav Orestis Stelmach
$Notice: $
================================================================================ */

#pragma once

#include "Nile/asset/asset.hh"

#include <glm/glm.hpp>

// ShaderSet - represent basically an OpenGL shader program object
// coposed at least of 2 shaders ( vertex and fragment ), geometry shader is
// optional, thous the name ShaderSet.
// Keeps track of ProgramID, also we set Uniform Variable though this object

namespace nile {

  class ShaderSet : public Asset {
  private:
    u32 m_programId;

  public:
    explicit ShaderSet( u32 id ) noexcept;

    ShaderSet &use() noexcept;

    void SetFloat( const char *name, f32 value, bool useShader = false ) noexcept;
    void SetInteger( const char *name, i32 value, bool useShader = false ) noexcept;
    void SetVector2f( const char *name, f32 x, f32 y, bool useShader = false ) noexcept;
    void SetVector2f( const char *name, const glm::vec2 &value, bool useShader = false ) noexcept;
    void SetVector3f( const char *name, f32 x, f32 y, f32 z, bool useShader = false ) noexcept;
    void SetVector3f( const char *name, const glm::vec3 &value, bool useShader = false ) noexcept;
    void SetVector4f( const char *name, f32 x, f32 y, f32 z, f32 w,
                      bool useShader = false ) noexcept;
    void SetVector4f( const char *name, const glm::vec4 &value, bool useShader = false ) noexcept;
    void SetMatrix4( const char *name, const glm::mat4 &matrix, bool useShader = false ) noexcept;
  };

}    // namespace nile
