#include "core/input_manager.hh"

namespace nile {

  InputManager *InputManager::m_instance = nullptr;

  InputManager::InputManager() noexcept {
    // Empty Constructor
  }

  InputManager *InputManager::getInstance() noexcept {
    if ( !m_instance ) {
      m_instance = new InputManager;
    }
    return m_instance;
  }

  void InputManager::destroy() noexcept {
    if ( m_instance ) {
      delete m_instance;
      m_instance = nullptr;
    }
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
