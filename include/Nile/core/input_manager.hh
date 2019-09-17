#pragma once

#include "Nile/core/nile.hh"
#include <SDL2/SDL.h>

#include <unordered_map>

namespace nile {

  class InputManager {
  private:
    SDL_Event m_event;
    bool m_shouldClose = false;

    std::unordered_map<SDL_Keycode, bool> m_pressedKeys;

  public:
    InputManager() noexcept;
    ~InputManager() noexcept;

    void update( [[maybe_unused]] f32 dt ) noexcept;

    [[nodiscard]] inline bool shouldClose() const noexcept {
      return m_shouldClose;
    }

    inline void terminateEngine() noexcept {
      this->m_shouldClose = true;
    }
    // Checks if user has pressed any specifc key
    [[nodiscard]] bool isKeyPressed( SDL_Keycode key ) const noexcept {
      auto it = m_pressedKeys.find( key );
      if ( it != m_pressedKeys.end() ) {
        return it->second;
      }
      return false;
    }

    // Checks if specifc key has released
    [[nodiscard]] bool isKeyHasReleased( SDL_Keycode key ) const noexcept {
      return !isKeyPressed( key );
    }
  };

}    // namespace nile
