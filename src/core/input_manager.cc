/* ================================================================================
$File: input_manager.cc
$Date: $
$Revision: $
$Creator: Rostislav Orestis Stelmach
$Notice: $
================================================================================ */

#include "Nile/core/input_manager.hh"
#include "Nile/core/settings.hh"

namespace nile {

  InputManager::InputManager( const std::shared_ptr<Settings> &settings ) noexcept
      : m_settings( settings ) {
    // Empty constructor
  }

  InputManager::~InputManager() noexcept {
    // Empty Destructor
  }

  void InputManager::processInGameEvents() noexcept {
    switch ( m_event.type ) {
      case SDL_KEYDOWN:
        this->pressKey( m_event.key.keysym.sym );
        break;
      case SDL_KEYUP:
        this->releaseKey( m_event.key.keysym.sym );
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
      case SDL_MOUSEWHEEL:
        m_mouseVerticalWheel = m_event.wheel.y;
        m_mouseHorizontalWheel = m_event.wheel.x;
        break;
      default:
        break;
    }
  }

  void InputManager::changeProgramMode() noexcept {
    if ( ( m_event.type == SDL_KEYDOWN ) && ( m_event.key.keysym.sym == SDLK_F1 ) ) {
      if ( m_settings->getProgramMode() == ProgramMode::EDITOR_MODE )
        m_settings->setProgramMode( ProgramMode::GAME_MODE );
      else
        m_settings->setProgramMode( ProgramMode::EDITOR_MODE );
    }

    if ( ( m_event.type == SDL_KEYDOWN ) && ( m_event.key.keysym.sym == SDLK_ESCAPE ) ) {
      m_shouldClose = true;
    }
    m_settings->setProgramMode( ProgramMode::EDITOR_MODE );
  }

  void InputManager::update( [[maybe_unused]] f32 dt ) noexcept {

    for ( const auto &i : m_pressedKeys ) {
      m_previousPressed[ i.first ] = i.second;
    }

    while ( SDL_PollEvent( &m_event ) ) {

      if ( m_event.type == SDL_QUIT ) {
        m_shouldClose = true;
      }

      // Chagne ProgramMode from Editor to Game mode or visa versa
      // this is used becase we are blocking user input to game mode when
      // we are in editor mode, or the opposite
      this->changeProgramMode();

      this->processInGameEvents();

      SDL_GetMouseState( &m_mousePosition.x, &m_mousePosition.y );
    }
  }

  void InputManager::pressKey( SDL_Keycode key ) noexcept {
    m_pressedKeys[ key ] = true;
  }

  void InputManager::releaseKey( SDL_Keycode key ) noexcept {
    m_pressedKeys[ key ] = false;
  }


}    // namespace nile
