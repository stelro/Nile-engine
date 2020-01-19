/* ================================================================================
$File: shaderset.hh
$Date: $
$Revision: $
$Creator: Rostislav Orestis Stelmach
$Notice: $
================================================================================ */

#pragma once

#include "Nile/asset/asset.hh"
#include "Nile/log/log.hh"

#include <glm/glm.hpp>

#if defined( TOOLS_ENABLED )
#include <chrono>
#include <filesystem>
#endif

// ShaderSet - represent basically an OpenGL shader program object
// coposed at least of 2 shaders ( vertex and fragment ), geometry shader is
// optional, thous the name ShaderSet.
// Keeps track of ProgramID, also we set Uniform Variable though this object

namespace nile {

#if defined( TOOLS_ENABLED )

  struct FileSize {
    std::uintmax_t vertex;
    std::uintmax_t fragment;
    std::uintmax_t geometry;
  };

  struct TimeStamp {
    std::filesystem::file_time_type vertex;
    std::filesystem::file_time_type fragment;
    std::filesystem::file_time_type geometry;
  };

#endif

  class AssetManagerHelper;

  class ShaderSet : public Asset {

    friend class AssetManagerHelper;

  private:
#if defined( TOOLS_ENABLED )
    FileSize m_fileSizes;
    TimeStamp m_timeStamps;
#endif

    u32 *m_programId;

    std::string m_vShaderPath;
    std::string m_fShaderPath;
    std::string m_gShaderPath;

    void setProgramId( u32 id ) noexcept;

  public:
    explicit ShaderSet( u32 id, const std::string &vShaderPath, const std::string &fShaderPath,
                        const std::string &gShaderPath ) noexcept;
    ~ShaderSet() noexcept;

    void resetAsset() noexcept;

    ShaderSet &use() noexcept;

    u32 getProgramId() const {
      return *m_programId;
    }

#ifdef TOOLS_ENABLED

    void setFileSize( const FileSize &fileSizes ) noexcept {
      m_fileSizes = fileSizes;
    }

    void setTimeStamp( const TimeStamp &timeStamps ) noexcept {
      m_timeStamps = timeStamps;
    }

    FileSize getFileSizes() const noexcept {
      return m_fileSizes;
    }

    TimeStamp getTimeStamps() const noexcept {
      return m_timeStamps;
    }

#endif

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

    std::string getVshaderPath() const noexcept;
    std::string getFShaderPath() const noexcept;
    std::string getGShaderPath() const noexcept;
  };

}    // namespace nile
