#pragma once

#include "Nile/core/types.hh"


// Inspred by lazyFoo tutorials timer class

namespace nile {

  class Timer {
  private:
    u32 m_startTicks = 0;
    u32 m_pausedTicks = 0;

    bool m_paused = false;
    bool m_started = false;

  public:
    Timer() noexcept = default;

    // Clock Actions
    void start() noexcept;
    void stop() noexcept;
    void pause() noexcept;
    void unpause() noexcept;

    u32 getTicks() const noexcept;

    bool isStarted() const noexcept;
    bool isPaused() const noexcept;
  };

}    // namespace nile
