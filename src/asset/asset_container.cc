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
      return it->second;
    }

    // Asset doesn't exist in the container
    return nullptr;
  }

  void AssetContainer::unloadAsset( const std::string &assetName ) noexcept {
    auto it = m_assets.find( assetName );
    if ( it != m_assets.end() ) {
      this->removeAsset( assetName );
    } else {
      log::error( "%s asset not found to unload it!\n", assetName.c_str() );
    }
  }

  void AssetContainer::removeAsset( const std::string &assetName ) noexcept {
    auto it = m_assets.find( assetName );
    if ( it != m_assets.end() ) {
      // @keep a eye on this
      it->second.reset();
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
      iter.second.reset();
    }

    m_assets.clear();
  }

  AssetContainer::assets_iter AssetContainer::begin() noexcept {
    return m_assets.begin();
  }

  const AssetContainer::const_assets_iter AssetContainer::begin() const noexcept {
    return m_assets.begin();
  }

  const AssetContainer::const_assets_iter AssetContainer::cbegin() const noexcept {
    return m_assets.cbegin();
  }

  AssetContainer::assets_iter AssetContainer::end() noexcept {
    return m_assets.end();
  }

  const AssetContainer::const_assets_iter AssetContainer::end() const noexcept {
    return m_assets.end();
  }

  const AssetContainer::const_assets_iter AssetContainer::cend() const noexcept {
    return m_assets.cend();
  }

  // void AssetContainer::replace( const std::string &name, Asset *asset ) noexcept {
  //
  //   auto it = m_assets.find( name );
  //   if ( it != m_assets.end() ) {
  //     // @keep a eye on this
  //     auto tmp = it->second;
  //     it->second = asset;
  //
  //     delete tmp;
  //   } else {
  //     log::print( "%s NOT FOUND!!!\n", name.c_str() );
  //   }
  // }
  //
}    // namespace nile
