#include "Nile/core/input_manager.hh"

namespace nile {

  InputManager::InputManager() noexcept {
    // Empty constructor
  }

  InputManager::~InputManager() noexcept {
    // Empty Destructor
  }

  void InputManager::update( [[maybe_unused]] f32 dt ) noexcept {

    while ( SDL_PollEvent( &m_event ) ) {
      if ( m_event.type == SDL_QUIT ) {
        m_shouldClose = true;
      }

      switch ( m_event.type ) {
        case SDL_KEYDOWN:
          m_pressedKeys[ m_event.key.keysym.sym ] = true;
          break;
        case SDL_KEYUP:
          m_pressedKeys[ m_event.key.keysym.sym ] = false;
          break;
        default:
          break;
      }
    }
  }

}    // namespace nile
