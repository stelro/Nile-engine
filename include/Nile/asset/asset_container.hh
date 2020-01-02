/* ================================================================================
$File: asset_container.hh
$Date: $
$Revision: $
$Creator: Rostislav Orestis Stelmach
$Notice: $
================================================================================ */

#pragma once

#include "Nile/core/types.hh"

#include <memory>
#include <unordered_map>

namespace nile {

  class Asset;

  class AssetContainer final {
  private:
    std::unordered_map<std::string, std::shared_ptr<Asset>> m_assets;

  public:
    using assets_iter = std::unordered_map<std::string, std::shared_ptr<Asset>>::iterator;
    using const_assets_iter =
        std::unordered_map<std::string, std::shared_ptr<Asset>>::const_iterator;

    AssetContainer() = default;

    ~AssetContainer() noexcept;

    // make assetcontainer iteratable
    assets_iter begin() noexcept;
    const const_assets_iter begin() const noexcept;
    const const_assets_iter cbegin() const noexcept;

    assets_iter end() noexcept;
    const const_assets_iter end() const noexcept;
    const const_assets_iter cend() const noexcept;

    bool addAsset( const std::string &assetName, const std::shared_ptr<Asset> &assset ) noexcept;

    std::shared_ptr<Asset> getAsset( const std::string &assetName ) const noexcept;

    // This method will decrease reference of an asset,
    // if the asset is does not used by any subsystem, then
    // it will be deleted
    void unloadAsset( const std::string &assetName ) noexcept;

    // This mothod will remove the asset entirly from the container
    // ( this method doesn't care if the particluar asset is still in use
    // or have reference counter above 1 )
    void removeAsset( const std::string &assetName ) noexcept;

    usize getSize() const noexcept;

    usize getMaxSize() const noexcept;

    bool isEmpty() const noexcept;

    bool isAssetExist( const std::string &assetName ) const noexcept;

    void clearAll() noexcept;
  };

}    // namespace nile
