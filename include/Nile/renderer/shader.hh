#pragma once

#include "Nile/core/nile.hh"
#include <glm/glm.hpp>
#include <string_view>

namespace nile {

  // General purpose shader object. Compiles from file, generates
  // compile / link-time errors messages and hosts several utility
  // functions for easy managment.

  class Shader {
  private:
    // Current shader state
    u32 m_id;

    // Checks if compilation or linking failed, and if so, print the error logs
    void checkCompileErrors( u32 object, std::string_view type ) noexcept;

  public:
    Shader() noexcept = default;
    ~Shader() noexcept;
    // Set the shader as active
    Shader &use() noexcept;

    // Compiles the shader from given shouce code
    // @note: geomtry source shader is optional
    void compile( const char *vertexSource, const char *fragmentSource,
                  const char *geometrySource = nullptr ) noexcept;

    // Utility functions
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
