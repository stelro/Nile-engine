/* ================================================================================
$File: font_loader.hh
$Date: 16 Nov 2019 $
$Revision: $
$Creator: Rostislav Orestis Stelmach
$Notice: $
================================================================================ */

#pragma once

#include "Nile/asset/subsystem/asset_loader.hh"
#include "Nile/renderer/font.hh"

namespace nile {

  template <>
  class AssetLoader<Font> final {
  public:
    std::shared_ptr<Font> operator()(const std::string& fileName, const std::string& filePath) noexcept;
  };

}    // namespace nile
