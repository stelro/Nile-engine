#pragma once

#include <memory>
#include <mutex>
#include <thread>

namespace nile {

  class AssetManager;
  class Asset;
  class ShaderSet;

  class AssetManagerHelper {
  private:
    std::shared_ptr<AssetManager> m_assetManager;


  public:
    AssetManagerHelper( const std::shared_ptr<AssetManager> &assetManager ) noexcept;
    void operator ()() noexcept;
    void reloadShaders() noexcept;

    bool checkShaderForChange();
  };

}    // namespace nile
