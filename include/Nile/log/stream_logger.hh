#pragma once

#include "Nile/log/log_type.hh"

namespace nile {


  constexpr const char *CONSOLE_COLOR_RED = "\033[0;31m";
  constexpr const char *CONSOLE_COLOR_GREEN = "\033[0;32m";
  constexpr const char *CONSOLE_COLOR_YELLOW = "\033[0;33m";
  constexpr const char *CONSOLE_COLOR_MAGENTA = "\033[0;35m";
  constexpr const char *CONSOLE_COLOR_RESET = "\033[0m";

  class StreamLogger {
  public:
    static void printToStream( const char *msg, LogType type ) noexcept;
  };

}    // namespace nile
