#pragma once

#include "Nile/core/timer.hh"

#include <spdlog/spdlog.h>

#include <chrono>
#include <iostream>

namespace nile {

  class BenchmarkTimer {
  private:
    std::string rutine_name_;
    Timer timer;

  public:
    BenchmarkTimer( std::string name ) noexcept
        : rutine_name_( std::move(name) ) {
      timer.start();
    }

    ~BenchmarkTimer() noexcept {
      auto ticks = timer.getTicks();
      timer.stop();

      spdlog::debug( "(benchmark) Rutine {} exectued and finished in {:.3f} seconds.", rutine_name_, ( ticks / 1000.f ) );
    }
  };


}    // namespace nile
