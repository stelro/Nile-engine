/* ================================================================================
$File: input_manager.hh
$Date: $
$Revision: $
$Creator: Rostislav Orestis Stelmach
$Notice: $
================================================================================ */

#pragma once

#include "Nile/core/types.hh"
#include <SDL2/SDL.h>
#include <glm/glm.hpp>

#include <unordered_map>

namespace nile {

  class InputManager {
  private:
    SDL_Event m_event;
    bool m_shouldClose = false;
    glm::ivec2 m_mousePosition;
    // Mouse wheel events
    // if y > 0 then it's scroll up, if < 0 it's scroll down
    i32 m_mouseVerticalWheel = 0;
    // if x > 0 its scroll right and if < 0 its scroll left
    i32 m_mouseHorizontalWheel = 0;

    struct {
      bool left = false;
      bool right = false;
      bool middle = false;
      bool button_x1 = false;
      bool button_x2 = false;
    } m_mouseButtonEvents;

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

    glm::ivec2 getMousePos() const noexcept {
      return m_mousePosition;
    }

    bool mouseRightPressed() const noexcept {
      return m_mouseButtonEvents.right;
    }

    bool mouseLeftPressed() const noexcept {
      return m_mouseButtonEvents.left;
    }

    bool mouseMiddlePressed() const noexcept {
      return m_mouseButtonEvents.middle;
    }

    i32 getVerticalWheel() const noexcept {
      return m_mouseVerticalWheel;
    }

    i32 getHorizontalWheel() const noexcept {
      return m_mouseHorizontalWheel;
    }
  };

}    // namespace nile
