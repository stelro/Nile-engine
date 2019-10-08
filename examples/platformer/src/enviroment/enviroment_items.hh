#pragma once

#include <Nile/2d/2d_camera.hh>
#include <Nile/2d/sprite.hh>
#include <Nile/2d/sprite_renderer.hh>
#include <Nile/2d/sprite_sheet.hh>
#include <Nile/2d/spritesheet_context.hh>
#include <Nile/asset/asset_manager.hh>
#include <Nile/core/nile.hh>
#include <Nile/core/settings.hh>
#include <memory>
#include <vector>

namespace platformer {

  using nile::f32;

  class EnviromentItems {
  private:
    std::unique_ptr<nile::SpriteSheet> m_torchSpriteSheet;

    std::shared_ptr<nile::Settings> m_settings;
    std::shared_ptr<nile::AssetManager> m_assetManager;
    std::shared_ptr<nile::SpriteRenderer> m_spriteRenderer;
    std::shared_ptr<nile::Camera2D> m_camera;
    std::vector<nile::Sprite> m_staticSprites;

  public:
    EnviromentItems( const std::shared_ptr<nile::Settings> &settings,
                     const std::shared_ptr<nile::AssetManager> &manager,
                     const std::shared_ptr<nile::SpriteRenderer> &renderer,
                     const std::shared_ptr<nile::Camera2D> &camera ) noexcept;

    void initialize() noexcept;
    void update( f32 deltaTime ) noexcept;
    void draw( f32 deltaTime ) noexcept;
  };

}    // namespace platformer
