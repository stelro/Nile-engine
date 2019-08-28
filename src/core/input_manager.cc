#include "core/input_manager.hh"

namespace nile {

  void InputManager::update( [[maybe_unused]] f32 dt ) noexcept {

    while ( SDL_PollEvent(&m_event) ) {
      if ( m_event.type == SDL_QUIT) {
        m_shouldClose = true;
      }
    }
      
  }

}    // namespace nile
