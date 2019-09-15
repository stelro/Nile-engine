#pragma once

#include "Nile/2d/sprite_sheet.hh"
#include "Nile/core/nile.hh"
#include <memory>

namespace nile {

  class Settings;
  class SpriteRenderer;
  class SpriteSheetContext;
  class FontRenderer;
  class InputManager;
  class Camera2D;

  class Game {
    OBSERVER
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

    std::shared_ptr<Settings> m_settings;
    std::unique_ptr<SpriteRenderer> m_spriteRenderer;
    std::unique_ptr<Camera2D> m_camera;
    std::unique_ptr<SpriteSheetContext> m_knightSprite;
    std::unique_ptr<FontRenderer> m_fontRenderer;

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

    // Default texture oriantion of the main hero ( north-east )
    TextureOrientation m_heroOriantaion = TextureOrientation::NE;

  public:
    Game( std::shared_ptr<Settings> settings ) noexcept;
    ~Game() noexcept;
    void init() noexcept;
    void update( float dt ) noexcept;
    void render( float dt ) noexcept;
  };

}    // namespace nile
