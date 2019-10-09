/* ================================================================================
$File: shaderset_builder.hh
$Date: $
$Revision: $
$Creator: Rostislav Orestis Stelmach
$Notice: $
================================================================================ */

#pragma once

#include "Nile/core/types.hh"
#include "Nile/asset/builder/asset_builder.hh"
#include "Nile/renderer/shaderset.hh"

#include <string_view>

namespace nile::AssetBuilder {

  enum class ShaderType
  {
    VERTEX,
    FRAGMENT,
    GEOMETRY,
    TESSALATION,
    COMPUTE,
    PROGRAM,
    NONE
  };

  template <>
  class Builder<ShaderSet> final {
  private:
    u32 loadShaderFromFile( std::string_view vshaderFile, std::string_view fshaderFile,
                            std::string_view gshaderFile ) noexcept;

    u32 compileShaders( const char *vSource, const char *fSource, const char *gSource ) noexcept;
    void checkForCompileErrors( u32 object, ShaderType type ) noexcept;

    std::string m_vertexPath {};
    std::string m_fragmentPath {};
    std::string m_geometryPath {};

  public:
    Builder() = default;
    Builder( Builder && ) = default;
    Builder( const Builder & ) = default;
    Builder &operator=( Builder && ) = default;
    Builder &operator=( const Builder & ) = default;


    Builder &setVertexPath( const std::string &value ) noexcept;
    Builder &setFragmentPath( const std::string &value ) noexcept;
    Builder &setGeometryPath( const std::string &value ) noexcept;

    [[nodiscard]] ShaderSet *build() noexcept;
  };

}    // namespace nile::AssetBuilder
