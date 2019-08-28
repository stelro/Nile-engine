#pragma once

#include <memory>

namespace nile {

  class Settings;
  class SpriteRenderer;
  class InputManager;

  class Game {
  private:
    std::shared_ptr<Settings> m_settings;
    std::unique_ptr<SpriteRenderer> m_spriteRenderer;
    
    // Singleton instance
    InputManager* m_inputManager;

  public:
    Game( std::shared_ptr<Settings> settings ) noexcept;
    ~Game() noexcept;
    void init() noexcept;
    void update( float dt ) noexcept;
    void render( float dt ) noexcept;
  };

}    // namespace nile
