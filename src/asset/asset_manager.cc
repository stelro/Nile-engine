/* ================================================================================
$File: asset_manager.cc
$Date: $
$Revision: 28 Nov 2019
$Creator: Rostislav Orestis Stelmach
$Notice: Replace AssetLoaders classes registration within a map container
         with a assetLoader classes specialization functor
================================================================================ */


#include "Nile/asset/asset_manager.hh"
#include "Nile/asset/asset_container.hh"

namespace nile {

  AssetManager::AssetManager() noexcept
      : m_assetContainer( new AssetContainer() ) {}

  AssetManager::~AssetManager() noexcept {
    delete m_assetContainer;
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

  AssetContainer::assets_iter AssetManager::begin() noexcept {
    return m_assetContainer->begin();
  }

  const AssetContainer::const_assets_iter AssetManager::begin() const noexcept {
    return m_assetContainer->begin();
  }

  const AssetContainer::const_assets_iter AssetManager::cbegin() const noexcept {
    return m_assetContainer->cbegin();
  }

  AssetContainer::assets_iter AssetManager::end() noexcept {
    return m_assetContainer->end();
  }

  const AssetContainer::const_assets_iter AssetManager::end() const noexcept {
    return m_assetContainer->end();
  }

  const AssetContainer::const_assets_iter AssetManager::cend() const noexcept {
    return m_assetContainer->cend();
  }

}    // namespace nile
