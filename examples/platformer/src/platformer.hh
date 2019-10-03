#pragma once

#include <Nile/2d/2d_camera.hh>
#include <Nile/2d/sprite_renderer.hh>
#include <Nile/2d/spritesheet_context.hh>
#include <Nile/application/game.hh>
#include <Nile/core/input_manager.hh>
#include <Nile/core/settings.hh>
#include <Nile/platform/game_host.hh>
#include <Nile/renderer/font_renderer.hh>
#include <Nile/asset/asset_manager.hh>

#include <GL/glew.h>
#include <glm/gtc/matrix_transform.hpp>
#include "npc/main_character.hh"
#include "enviroment/enviroment_items.hh"

#include <memory>

namespace platformer {

  using nile::f32;

  class MainCharacter;
  class Platformer : public nile::Game {
  private:
    std::shared_ptr<nile::GameHost> m_gameHost;
    std::shared_ptr<nile::InputManager> m_inputManager;
    std::shared_ptr<nile::BaseRenderer> m_renderer;
    std::shared_ptr<nile::Settings> m_settings;
    std::shared_ptr<nile::AssetManager> m_assetManager;

    std::shared_ptr<nile::SpriteRenderer> m_spriteRenderer;
    std::shared_ptr<nile::Camera2D> m_camera;
    std::shared_ptr<nile::FontRenderer> m_fontRenderer;

    std::unique_ptr<MainCharacter> m_mainCharacter;
    std::unique_ptr<EnviromentItems> m_enviromentItems;
    

  public:
    Platformer( const std::shared_ptr<nile::GameHost> &gameHost ) noexcept;
    ~Platformer() = default;
    virtual void initialize() noexcept override;
    virtual void draw( f32 deltaTime ) noexcept override;
    virtual void update( f32 deltaTime ) noexcept override;
  };

}    // namespace platformer

