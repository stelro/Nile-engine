/* ================================================================================
$File: asset_container.cc
$Date: $
$Revision: $
$Creator: Rostislav Orestis Stelmach
$Notice: $
================================================================================ */

#include "Nile/asset/asset_container.hh"
#include "Nile/asset/asset.hh"
#include "Nile/log/log.hh"

namespace nile {

  AssetContainer::~AssetContainer() noexcept {
    this->clearAll();
  }

  bool AssetContainer::addAsset( const std::string &assetName,
                                 const std::shared_ptr<Asset> &asset ) noexcept {
    auto inserted = m_assets.insert( std::make_pair( assetName, asset ) );
    return std::get<bool>( inserted );
  }

  std::shared_ptr<Asset> AssetContainer::getAsset( const std::string &assetName ) const noexcept {

    auto it = m_assets.find( assetName );

    if ( it != m_assets.end() ) {
      if ( auto spt = it->second.lock() )
        return spt;
      else
        log::error( "%s asset has expired\n" );
    }

    // Asset doesn't exist in the container
    return nullptr;
  }

  void AssetContainer::unloadAsset( const std::string &assetName ) noexcept {
    auto it = m_assets.find( assetName );
    if ( it != m_assets.end() ) {
      if ( !it->second.expired() )
        this->removeAsset( assetName );
    }
  }

  void AssetContainer::removeAsset( const std::string &assetName ) noexcept {
    auto it = m_assets.find( assetName );
    if ( it != m_assets.end() ) {
      // @keep a eye on this
      it->second.reset();
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
      iter.second.reset();
    }

    m_assets.clear();
  }

}    // namespace nile
