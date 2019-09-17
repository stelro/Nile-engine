#pragma once

#include <Nile/2d/2d_camera.hh>
#include <Nile/2d/spritesheet_context.hh>
#include <Nile/core/input_manager.hh>
#include <Nile/core/nile.hh>
#include <Nile/core/settings.hh>
#include <Nile/core/signal.hh>

#include <memory>

namespace platformer {

  using nile::f32;

  class MainCharacter {
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

    std::shared_ptr<nile::Settings> m_settings;
    std::shared_ptr<nile::InputManager> m_inputManager;
    std::shared_ptr<nile::SpriteSheetContext> m_spriteContext;
    std::shared_ptr<nile::Camera2D> m_camera;

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
    MainCharacter( const std::shared_ptr<nile::Settings> &settings,
                   const std::shared_ptr<nile::InputManager> &inputManager,
                   const std::shared_ptr<nile::Camera2D> &camera ) noexcept;
    ~MainCharacter() noexcept;

    void initialize() noexcept;
    void update( f32 deltaTime ) noexcept;
    void draw( f32 deltaTime ) noexcept;
  };


}    // namespace platformer

