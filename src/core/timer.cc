#include "Nile/core/timer.hh"
#include <SDL2/SDL.h>

namespace nile {

  // Clock Actions
  void Timer::start() noexcept {
    m_started = true;
    m_paused = false;
    m_startTicks = SDL_GetTicks();
    m_pausedTicks = 0;
  }

  void Timer::stop() noexcept {
    m_started = false;
    m_paused = false;
    m_startTicks = 0;
    m_pausedTicks = 0;
  }

  void Timer::pause() noexcept {
    if ( m_started && !m_paused ) {
      m_paused = true;
      m_pausedTicks = SDL_GetTicks() - m_startTicks;
      m_startTicks = 0;
    }
  }

  void Timer::unpause() noexcept {
    if ( m_started && m_paused ) {
      m_paused = false;
      m_startTicks = SDL_GetTicks() - m_pausedTicks;
      m_pausedTicks = 0;
    }
  }

  u32 Timer::getTicks() const noexcept {
    u32 time = 0;

    if ( m_started ) {
      if ( m_paused ) {
        time = m_pausedTicks;
      } else {
        time = SDL_GetTicks() - m_startTicks;
      }
    }

    return time;
  }

  bool Timer::isStarted() const noexcept {
    return m_started;
  }

  bool Timer::isPaused() const noexcept {
    return m_paused;
  }


}    // namespace nile
