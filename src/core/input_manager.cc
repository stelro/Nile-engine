/* ================================================================================
$File: input_manager.cc
$Date: $
$Revision: $
$Creator: Rostislav Orestis Stelmach
$Notice: $
================================================================================ */

#include "Nile/core/input_manager.hh"
#include "Nile/core/logger.hh"

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
        case SDL_MOUSEBUTTONDOWN:
          switch ( m_event.button.button ) {
            case SDL_BUTTON_LEFT:
              m_mouseButtonEvents.left = true;
              break;
            case SDL_BUTTON_RIGHT:
              m_mouseButtonEvents.right = true;
              break;
            case SDL_BUTTON_MIDDLE:
              m_mouseButtonEvents.middle = true;
              break;
            case SDL_BUTTON_X1:
              m_mouseButtonEvents.button_x1 = true;
              break;
            case SDL_BUTTON_X2:
              m_mouseButtonEvents.button_x2 = true;
              break;
            default:
              break;
          }
          break;
        case SDL_MOUSEBUTTONUP:
          switch ( m_event.button.button ) {
            case SDL_BUTTON_LEFT:
              m_mouseButtonEvents.left = false;
              break;
            case SDL_BUTTON_RIGHT:
              m_mouseButtonEvents.right = false;
              break;
            case SDL_BUTTON_MIDDLE:
              m_mouseButtonEvents.middle = false;
              break;
            case SDL_BUTTON_X1:
              m_mouseButtonEvents.button_x1 = false;
              break;
            case SDL_BUTTON_X2:
              m_mouseButtonEvents.button_x2 = false;
              break;
            default:
              break;
          }

          break;
        default:
          break;
      }
    }

    SDL_GetMouseState( &m_mousePosition.x, &m_mousePosition.y );
  }

}    // namespace nile
