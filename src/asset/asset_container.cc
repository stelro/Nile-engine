/* ================================================================================
$File: asset_container.cc
$Date: $
$Revision: $
$Creator: Rostislav Orestis Stelmach
$Notice: $
================================================================================ */

#include "Nile/asset/asset_container.hh"
#include "Nile/asset/asset.hh"

namespace nile {

  AssetContainer::~AssetContainer() noexcept {
    this->clearAll();
  }

  bool AssetContainer::addAsset( const std::string &assetName, Asset *asset ) noexcept {
    auto inserted = m_assets.insert( std::make_pair( assetName, asset ) );
    return std::get<bool>( inserted );
  }

  Asset *AssetContainer::getAsset( const std::string &assetName ) const noexcept {
    auto it = m_assets.find( assetName );
    if ( it != m_assets.end() ) {
      // Increment the reference by one of the asset
      it->second->m_reference->inc();
      return it->second;
    }

    // Asset doesn't exist in the container
    return nullptr;
  }

  void AssetContainer::unloadAsset( const std::string &assetName ) noexcept {
    auto it = m_assets.find( assetName );
    if ( it != m_assets.end() ) {
      it->second->m_reference->dec();
      if ( it->second->getRefCount() <= 0 )
        this->removeAsset( assetName );
    }
  }

  void AssetContainer::removeAsset( const std::string &assetName ) noexcept {
    auto it = m_assets.find( assetName );
    if ( it != m_assets.end() ) {
      delete it->second;
      it->second = nullptr;
      m_assets.erase( it );
    }
  }

  usize AssetContainer::getSize() const noexcept {
    return m_assets.size();
  }

  usize AssetContainer::getMaxSize() const noexcept {
    return m_assets.max_size();
  }

  bool AssetContainer::isEmpty() const noexcept {
    return m_assets.empty();
  }

  bool AssetContainer::isAssetExist( const std::string &assetName ) const noexcept {
    return m_assets.find( assetName ) != m_assets.end();
  }

  void AssetContainer::clearAll() noexcept {
    for ( auto &iter : m_assets ) {
      delete iter.second;
      iter.second = nullptr;
    }

    m_assets.clear();
  }

}    // namespace nile
