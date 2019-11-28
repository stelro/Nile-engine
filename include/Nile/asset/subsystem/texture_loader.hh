/* ================================================================================
$File: texture_loader.hh
$Date: $
$Revision: $
$Creator: Rostislav Orestis Stelmach
$Notice: $
================================================================================ */

#pragma once

#include "Nile/asset/subsystem/asset_loader.hh"
#include "Nile/renderer/texture2d.hh"

namespace nile {

  template <>
  class AssetLoader<Texture2D> final {
    public:

    std::shared_ptr<Texture2D> operator()( const std::string &fileName,
                                           const std::string &filePath ) noexcept;
  };


}    // namespace nile

