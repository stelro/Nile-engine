#pragma once

#include "2d/sprite_sheet.hh"
#include "core/nile.hh"
#include <memory>

namespace nile {

  class Settings;
  class SpriteRenderer;
  class SpriteSheetContext;
  class InputManager;
  class Camera2D;

  class Game {
    OBSERVER
  private:
    enum class HeroStateEnum
    {
      IDLE,
      RUNNING,
      ATTACK
    };

    std::shared_ptr<Settings> m_settings;
    std::unique_ptr<SpriteRenderer> m_spriteRenderer;
    std::unique_ptr<Camera2D> m_camera;
    std::unique_ptr<SpriteSheetContext> m_knightSprite;

    // Singleton instance
    InputManager *m_inputManager;

    HeroStateEnum m_heroState = HeroStateEnum::IDLE;

    // This is the listener who listens to signals from
    // SpriteSheet class
    SpriteSheet::Animation_signal::Listener m_animationListener;

    // This is the animation slot function that is called in response to
    // a particular signal from animationListener
    std::function<void( bool )> m_animationSlot;
    bool m_shouldHaltTheEvents = false;

  public:
    Game( std::shared_ptr<Settings> settings ) noexcept;
    ~Game() noexcept;
    void init() noexcept;
    void update( float dt ) noexcept;
    void render( float dt ) noexcept;
  };

}    // namespace nile
