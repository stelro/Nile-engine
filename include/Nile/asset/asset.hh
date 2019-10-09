/* ================================================================================
$File: asset.hh
$Date: $
$Revision: $
$Creator: Rostislav Orestis Stelmach
$Notice: $
================================================================================ */

#pragma once

#include "Nile/core/reference.hh"

namespace nile {

  class AssetContainer;

  class Asset {
    friend class AssetContainer;

  private:
    Reference *m_reference;
    std::string m_fileName;
    std::string m_assetName;

  public:
    Asset() noexcept
        : m_reference( new Reference() ) {}
    virtual ~Asset() noexcept {
      delete m_reference;
    }

    void setFileName( const std::string &name ) noexcept {
      m_fileName = name;
    }

    void setAssetName( const std::string &name ) noexcept {
      m_assetName = name;
    }

    std::string getFileName() const noexcept {
      return m_fileName;
    }

    std::string getAssetName() const noexcept {
      return m_assetName;
    }

    u32 getRefCount() const noexcept {
      return m_reference->getRefCount();
    }

    void inc() noexcept {
      m_reference->inc();
    }

    void dec() noexcept {
      m_reference->dec();
    }
  };

}    // namespace nile
