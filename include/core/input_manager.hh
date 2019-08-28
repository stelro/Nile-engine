#pragma once

#include "core/nile.hh"

#include <SDL2/SDL.h>

namespace nile {

  class InputManager {
  private:
    SDL_Event m_event;
    bool m_shouldClose = false;

  public:
    void update( [[maybe_unused]] f32 dt ) noexcept;

    [[nodiscard]] inline bool shouldClose() const noexcept {
      return m_shouldClose;
    }
  };

}    // namespace nile
