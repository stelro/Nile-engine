/* ================================================================================
$File: asset_loader.hh
$Date: $
$Revision: $
$Creator: Rostislav Orestis Stelmach
$Notice: $
================================================================================ */

#pragma once
#include "Nile/core/reference.hh"

// AssetLoader base class - every derived class that inherits this base class
// should implement the logic of how each type of asset should be loaded
// (e.g, read file from disk, compile, etc..)

namespace nile {

  class Asset;

  class AssetLoader {
  protected:
    Reference *m_reference;

  public:
    AssetLoader() {}
    virtual ~AssetLoader() {}
    virtual Asset *loadAsset( const std::string &assetName,
                              const std::string &filePath ) noexcept = 0;

    virtual void unloadAsset( Asset* asset ) noexcept = 0;
  };

}    // namespace nile
