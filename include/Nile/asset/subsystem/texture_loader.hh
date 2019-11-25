/* ================================================================================
$File: texture_loader.hh
$Date: $
$Revision: $
$Creator: Rostislav Orestis Stelmach
$Notice: $
================================================================================ */

#pragma once

#include "Nile/asset/subsystem/asset_loader.hh"

namespace nile {

  class TextureLoader : public AssetLoader {
 private:
    bool m_alpha;

  public:
    TextureLoader( bool alpha = true ) noexcept;
    std::shared_ptr<Asset> loadAsset( const std::string &assetName,
                                      const std::string &filePath ) noexcept override;

    void unloadAsset( Asset *asset ) noexcept override;
  };

}    // namespace nile

