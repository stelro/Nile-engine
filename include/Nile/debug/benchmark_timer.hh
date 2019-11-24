#pragma once

#include "Nile/core/timer.hh"
#include "Nile/log/log.hh"

#include <chrono>
#include <iostream>

namespace nile {

  class BenchmarkTimer {
  private:
    std::string m_methodName;
    Timer timer;

  public:
    BenchmarkTimer( const std::string &name ) noexcept
        : m_methodName( name ) {
      timer.start();
    }

    ~BenchmarkTimer() noexcept {
      auto ticks = timer.getTicks();
      timer.stop();

      log::notice( "-- method %s needed %.3fs to execute\n", m_methodName.c_str(), ( ticks / 1000.f ) );
    }
  };


}    // namespace nile
