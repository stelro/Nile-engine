#pragma once

#include <Nile/2d/sprite_renderer.hh>
#include <Nile/2d/spritesheet_context.hh>
#include <Nile/application/game.hh>
#include <Nile/asset/asset_manager.hh>
#include <Nile/core/input_manager.hh>
#include <Nile/core/settings.hh>
#include <Nile/ecs/ecs_coordinator.hh>
#include <Nile/platform/game_host.hh>
#include <Nile/renderer/font_renderer.hh>

#include "enviroment/enviroment_items.hh"
#include "npc/main_character.hh"
#include <GL/glew.h>
#include <glm/gtc/matrix_transform.hpp>

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
    std::shared_ptr<nile::Coordinator> m_ecsCoordinator;

    std::shared_ptr<nile::SpriteRenderer> m_spriteRenderer;
    std::shared_ptr<nile::FontRenderer> m_fontRenderer;

    std::unique_ptr<MainCharacter> m_mainCharacter;
    std::unique_ptr<EnviromentItems> m_enviromentItems;

    nile::Entity m_cameraEntity;

    void initializeEcs() noexcept;
    void createAxisLines() noexcept;
    void test3d() noexcept;

    void processKeyboardEvents(f32 dt) noexcept;
    void processMouseEvents(f32 dt) noexcept;
    void processMouseScroll(f32 dt) noexcept;

    // Used for FPS-like camrea
    f32 m_lastX;
    f32 m_lastY;
    bool m_firstMouse = true;
    glm::ivec2 m_mouse_pos;

  public:
    Platformer( const std::shared_ptr<nile::GameHost> &gameHost ) noexcept;
    ~Platformer() = default;
    virtual void initialize() noexcept override;
    virtual void draw( f32 deltaTime ) noexcept override;
    virtual void update( f32 deltaTime ) noexcept override;
  };


}    // namespace platformer

