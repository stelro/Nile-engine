#pragma once

#include <Nile/2d/2d_camera.hh>
#include <Nile/2d/sprite_renderer.hh>
#include <Nile/2d/spritesheet_context.hh>
#include <Nile/application/game.hh>
#include <Nile/platform/game_host.hh>
#include <Nile/renderer/font_renderer.hh>
#include <Nile/renderer/shader.hh>
#include <Nile/resource/resource_manager.hh>
#include <Nile/core/signal.hh>
#include <Nile/core/input_manager.hh>
#include <Nile/core/settings.hh>

#include <GL/glew.h>
#include <glm/gtc/matrix_transform.hpp>

#include <memory>

namespace platformer {

  using nile::f32;


  class Platformer : public nile::Game {
  private:
    enum class HeroStateEnum
    {
      IDLE,
      RUNNING,
      ATTACK,
      JUMP_AND_FALL,
      ROLL_STRIP,
      SHIELD_STRIP
    };

    std::shared_ptr<nile::GameHost> m_gameHost;
    std::shared_ptr<nile::InputManager> m_inputManager;
    std::shared_ptr<nile::BaseRenderer> m_renderer;
    std::shared_ptr<nile::Settings> m_settings;

    std::shared_ptr<nile::SpriteRenderer> m_spriteRenderer;
    std::shared_ptr<nile::Camera2D> m_camera;
    std::shared_ptr<nile::SpriteSheetContext> m_knightSprite;
    std::shared_ptr<nile::FontRenderer> m_fontRenderer;

    HeroStateEnum m_heroState = HeroStateEnum::IDLE;

    // This is the listener who listens to signals from
    // SpriteSheet class
    nile::SpriteSheet::Animation_signal::Listener m_animationListener;

    // This is the animation slot function that is called in response to
    // a particular signal from animationListener
    std::function<void( bool )> m_animationSlot;
    bool m_shouldHaltTheEvents = false;

    // Default texture oriantion of the main hero ( north-east )
    nile::TextureOrientation m_heroOriantaion = nile::TextureOrientation::NE;

  public:
    Platformer( const std::shared_ptr<nile::GameHost> &gameHost ) noexcept;
    virtual void initialize() noexcept override;
    virtual void draw( f32 deltaTime ) noexcept override;
    virtual void update( f32 deltaTime ) noexcept override;
  };

}    // namespace platformer
