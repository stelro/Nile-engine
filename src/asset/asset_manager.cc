/* ================================================================================
$File: asset_manager.cc
$Date: $
$Revision: $
$Creator: Rostislav Orestis Stelmach
$Notice: $
================================================================================ */

#include "Nile/asset/asset_manager.hh"
#include "Nile/asset/asset_container.hh"

namespace nile {

  AssetManager::AssetManager() noexcept
      : m_assetContainer( new AssetContainer() ) {}

  AssetManager::~AssetManager() noexcept {
    delete m_assetContainer;
    clearLoaders();
  }

  bool AssetManager::isAssetExist( const std::string &assetName ) const noexcept {
    return m_assetContainer->isAssetExist( assetName );
  }

  usize AssetManager::getContainerSize() const noexcept {
    return m_assetContainer->getSize();
  }

  void AssetManager::unloadAsset( const std::string &assetName ) noexcept {
    m_assetContainer->unloadAsset( assetName );
  }

  void AssetManager::clearAll() noexcept {
    m_assetContainer->clearAll();
  }

  void AssetManager::clearLoaders() noexcept {
    for ( auto &i : m_loaders ) {
      delete i.second;
      i.second = nullptr;
    }
    m_loaders.clear();
  }

  void AssetManager::reloadAll() noexcept {
    // Empty function block
    // TODO(setl): should implement reloadall method
  }

  void AssetManager::reload( const std::string &assetName ) noexcept {
    // Empty function block
    // TODO(setl): should implement reloadall method
  }

  usize AssetManager::getLoadersCount() const noexcept {
    return m_loaders.size();
  }

}    // namespace nile
