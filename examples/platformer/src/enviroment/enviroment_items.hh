#pragma once

#include <Nile/2d/sprite_sheet.hh>
#include <Nile/asset/asset_manager.hh>
#include <Nile/core/nile.hh>
#include <Nile/core/settings.hh>
#include <Nile/2d/spritesheet_context.hh>

#include <memory>

namespace platformer {

  using nile::f32;

  class EnviromentItems {
  private:
    std::unique_ptr<nile::SpriteSheet> m_torchSpriteSheet;
    std::unique_ptr<nile::SpriteSheetContext> m_torchContext;

    std::shared_ptr<nile::Settings> m_settings;
    std::shared_ptr<nile::AssetManager> m_assetManager;
  public:
    EnviromentItems( const std::shared_ptr<nile::Settings> &settings,
                     const std::shared_ptr<nile::AssetManager> &manager ) noexcept;
    void initialize() noexcept;
    void update( f32 deltaTime ) noexcept;
    void draw( f32 deltaTime ) noexcept;
  };

}    // namespace platformer
