/* ================================================================================
$File: asset_manager.hh
$Date: $
$Revision: 28 Nov 2019
$Creator: Rostislav Orestis Stelmach
$Notice: Replace AssetLoaders classes registration within a map container
         with a assetLoader classes specialization functor
================================================================================ */

#pragma once

#include "Nile/asset/asset_container.hh"
#include "Nile/asset/builder/asset_builder.hh"
#include "Nile/asset/subsystem/asset_loader.hh"
#include "Nile/asset/subsystem/font_loader.hh"
#include "Nile/asset/subsystem/texture_loader.hh"
#include "Nile/core/assert.hh"
#include "Nile/log/log.hh"
#include "Nile/renderer/texture2d.hh"

#include <optional>
#include <unordered_map>

namespace nile {

  class Asset;

  class AssetManager final {
  private:
    AssetContainer *m_assetContainer;

  public:
    AssetManager() noexcept;
    ~AssetManager() noexcept;

    template <typename T>
    [[nodiscard]] std::shared_ptr<T> loadAsset( const std::string &assetName,
                                                const std::string &assetPath ) noexcept {

      auto asset = m_assetContainer->getAsset( assetName );

      if ( !asset ) {
        // We don't need any special check here to see if loader exist,
        // since the error we got is compile time error due the templates
        // and that's what we want
        AssetLoader<T> loader;
        asset = loader( assetName, assetPath );
        m_assetContainer->addAsset( assetName, asset );
      }
      return std::static_pointer_cast<T>( asset );
    }

    // Stores asset to the AssetContainer, which is created outside of assetManager
    // class, usually with some specific Builder, which was Created by the manager class
    // this method is useful for e.g ShaderSets which need more than 1 file to be
    // created, or 3D models which contains more than one meshes and
    // thous cannot be created directly by the AssetManager's loadAsset method
    template <typename T>
    std::shared_ptr<T> storeAsset( const std::string &assetName,
                                   const std::shared_ptr<Asset> &asset ) noexcept {
      if ( !m_assetContainer->isAssetExist( assetName ) ) {
        m_assetContainer->addAsset( assetName, asset );
      } else {
        log::print("already exist\n");
      }
      
      return this->getAsset<T>( assetName );
    }

    // get asset by assetName if exist, otherwise error will be logged to the
    // console, and nullptr will returned
    template <typename T>
    std::shared_ptr<T> getAsset( const std::string &assetName ) const noexcept {

      if ( auto asset = m_assetContainer->getAsset( assetName ) ) {
        return std::static_pointer_cast<T>( asset );
      }

      log::error( "Could not find asset: %s\n", assetName.c_str() );

      return nullptr;
    }

    // Creates and returns builder to the client
    template <typename T, typename... Args>
    AssetBuilder::Builder<T> createBuilder( Args &&... args ) noexcept {
      AssetBuilder::Builder<T> builder( std::forward<Args>( args )... );
      return builder;
    }

    void unloadAsset( const std::string &assetName ) noexcept;

    // Check if assetName exist in AssetContainer
    bool isAssetExist( const std::string &assetName ) const noexcept;

    usize getContainerSize() const noexcept;

    // Clear all the assets from the container
    void clearAll() noexcept;

    // abstraction of AssetContainer
    AssetContainer::assets_iter begin() noexcept;
    const AssetContainer::const_assets_iter begin() const noexcept;
    const AssetContainer::const_assets_iter cbegin() const noexcept;

    AssetContainer::assets_iter end() noexcept;
    const AssetContainer::const_assets_iter end() const noexcept;
    const AssetContainer::const_assets_iter cend() const noexcept;

  };

}    // namespace nile
