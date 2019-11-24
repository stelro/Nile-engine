/* ================================================================================
$File: asset_manager.hh
$Date: $
$Revision: $
$Creator: Rostislav Orestis Stelmach
$Notice: $
================================================================================ */

#pragma once

#include "Nile/asset/asset_container.hh"
#include "Nile/asset/builder/asset_builder.hh"
#include "Nile/asset/subsystem/asset_loader.hh"
#include "Nile/core/assert.hh"
#include "Nile/log/log.hh"

#include <optional>
#include <unordered_map>

namespace nile {

  class Asset;

  class AssetManager final {
  private:
    std::unordered_map<const std::type_info *, AssetLoader *> m_loaders;
    AssetContainer *m_assetContainer;

  public:
    AssetManager() noexcept;
    ~AssetManager() noexcept;

    // T is the type of asset, U is the type of the loader
    // Args ( optional ) is arguments passed to newly created Loader
    template <typename T, typename U, typename... Args>
    bool registerLoader( Args &&... args ) noexcept {

      auto beforeSize = m_loaders.size();

      m_loaders.insert( m_loaders.end(),
                        std::make_pair( &typeid( T ), new U( std::forward<Args>( args )... ) ) );

      return m_loaders.size() > beforeSize;
    }

    // Load and return Asset of type T
    // if Asset T doesn't exist in the assetContainer, and loader of type U<T> exist
    // then load and save the asset to the AssetContainer.
    // If loader of type U<T> doesn't exist, then assert will arise
    template <typename T>
    T *loadAsset( const std::string &assetName, const std::string &assetPath ) noexcept {

      auto asset = m_assetContainer->getAsset( assetName );

      if ( !asset ) {
        auto it = m_loaders.find( &typeid( T ) );
        if ( it != m_loaders.end() ) {
          asset = it->second->loadAsset( assetName, assetPath );
          m_assetContainer->addAsset( assetName, asset );
        } else {
          // Loader doesn't exist
          ASSERT_M( false, "Could not find loader" );
        }
      }

      return static_cast<T *>( asset );
    }

    // Stores asset to the AssetContainer, which is created outside of assetManager
    // class, usually with some specific Builder, which was Created by the manager class
    // this method is useful for e.g ShaderSets which need more than 1 file to be
    // created, or 3D models which contains more than one meshes and
    // thous cannot be created directly by the AssetManager's loadAsset method
    template <typename T>
    void storeAsset( const std::string &assetName, Asset *asset ) noexcept {
      if ( !m_assetContainer->isAssetExist( assetName ) ) {
        m_assetContainer->addAsset( assetName, asset );
      }
      return;
    }


    // get asset by assetName if exist, otherwise error will be logged to the
    // console, and nullptr will returned
    template <typename T>
    T *getAsset( const std::string &assetName ) const noexcept {

      if ( auto asset = m_assetContainer->getAsset( assetName ) ) {
        return static_cast<T *>( asset );
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

    // Reload all the assets ( this involves many operations to file/stream)
    void reloadAll() noexcept;

    void reload( const std::string &assetName ) noexcept;

    template <typename T>
    void removeLoader() noexcept {
      auto it = m_loaders.find( &typeid( T ) );
      if ( it != m_loaders.end() ) {
        delete it->second;
        m_loaders.erase( it );
      }
    }

    void clearLoaders() noexcept;

    usize getLoadersCount() const noexcept;
  };

}    // namespace nile
