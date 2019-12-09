#pragma once

#include "text_buffer.hh"

#include <Nile/application/game.hh>
#include <Nile/asset/asset_manager.hh>
#include <Nile/core/input_manager.hh>
#include <Nile/core/settings.hh>
#include <Nile/ecs/ecs_coordinator.hh>
#include <Nile/platform/game_host.hh>

#include <GL/glew.h>
#include <glm/gtc/matrix_transform.hpp>

#include <memory>

namespace platformer {

  using nile::f32;

  class Platformer : public nile::Game {
  private:
    std::shared_ptr<nile::GameHost> m_gameHost;
    std::shared_ptr<nile::InputManager> m_inputManager;
    std::shared_ptr<nile::BaseRenderer> m_renderer;
    std::shared_ptr<nile::Settings> m_settings;
    std::shared_ptr<nile::AssetManager> m_assetManager;
    std::shared_ptr<nile::Coordinator> m_ecsCoordinator;

    std::unique_ptr<TextBuffer> m_textBuffer;

    nile::Entity m_cameraEntity;
    nile::Entity m_lampEntity;

    nile::Entity m_testEntity;

    void drawStoneTiles() noexcept;
    void drawNanoModel() noexcept;
    void drawTextureFloor() noexcept;
    void drawContainers() noexcept;
    void drawGrass() noexcept;
    void drawWindows() noexcept;
    void drawFont() noexcept;
    void drawLigths() noexcept;

    void processKeyboardEvents( f32 dt ) noexcept;
    void processMouseEvents( f32 dt ) noexcept;
    void processMouseScroll( f32 dt ) noexcept;

    // Used for FPS-like camrea
    f32 m_lastX;
    f32 m_lastY;
    bool m_firstMouse = true;
    glm::ivec2 m_mouse_pos;

    glm::vec3 lightColor {1.0f, 1.0f, 1.0f};
    glm::vec3 lightPos {42.0f, 16.0f, 22.0f};

  public:
    Platformer( const std::shared_ptr<nile::GameHost> &gameHost ) noexcept;
    ~Platformer() = default;
    virtual void initialize() noexcept override;
    virtual void draw( f32 deltaTime ) noexcept override;
    virtual void update( f32 deltaTime ) noexcept override;
  };


}    // namespace platformer

