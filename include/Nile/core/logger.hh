/* ================================================================================
$File: logger.hh
$Date: $
$Revision: $
$Creator: Rostislav Orestis Stelmach
$Notice: $
================================================================================ */

#pragma once

#include <cstdarg>
#include <fstream>
#include <iostream>

// TODO: create a regular methods that outputs mesg to a std stream
// also create a method that takes as input a stream ( file for example )
// and writes it with specific format and time

namespace nile {

  namespace log {

    //
    // [0;xx used for regular font format
    // [1;xx used for bold font format
    //

    constexpr const char *CONSOLE_COLOR_RED = "\033[0;31m";
    constexpr const char *CONSOLE_COLOR_GREEN = "\033[0;32m";
    constexpr const char *CONSOLE_COLOR_YELLOW = "\033[0;33m";
    constexpr const char *CONSOLE_COLOR_MAGENTA = "\033[0;35m";
    constexpr const char *CONSOLE_COLOR_RESET = "\033[0m";

    enum class LogType
    {
      INFO,
      ERROR,
      WARNING,
      FATAL,
      PRINT
    };

    template <typename... Args>
    extern std::string generic_print_func( const std::string &str, va_list args ) noexcept {
      va_list argsCopy;
      va_copy( argsCopy, args );
      auto size = vsnprintf( nullptr, 0, str.c_str(), args ) + 1;

      std::string tmp;
      tmp.resize( static_cast<long unsigned>( size ) );
      vsnprintf( &tmp[ 0 ], static_cast<size_t>( size ), str.c_str(), argsCopy );
      tmp.pop_back();    // remove trailing NULL

      return tmp;
    }

    template <typename... Args>
    extern size_t process_log( LogType type, const char *format, va_list args ) noexcept {

      std::string info_string;
      std::string args_str = generic_print_func( format, args );

      /**
         Maybe abstract somehow this swith statement away?
       */
      switch ( type ) {
        case LogType::INFO:
          info_string += "[ ";
          info_string += CONSOLE_COLOR_GREEN;
          info_string += "INFO";
          info_string += CONSOLE_COLOR_RESET;
          info_string += " ] : ";
          info_string += args_str;
          std::cout << info_string;
          break;
        case LogType::ERROR:
          info_string += "[ ";
          info_string += CONSOLE_COLOR_RED;
          info_string += "ERROR";
          info_string += CONSOLE_COLOR_RESET;
          info_string += " ] : ";
          info_string += args_str;
          std::cerr << info_string;
          break;
        case LogType::WARNING:
          info_string += "[ ";
          info_string += CONSOLE_COLOR_YELLOW;
          info_string += "WARNING";
          info_string += CONSOLE_COLOR_RESET;
          info_string += " ] : ";
          info_string += args_str;
          std::cout << info_string;
          break;
        case LogType::FATAL:
          info_string += "[ ";
          info_string += CONSOLE_COLOR_RED;
          info_string += "FATAL";
          info_string += CONSOLE_COLOR_RESET;
          info_string += " ] : ";
          info_string += args_str;
          std::cout << info_string;
          break;
        case LogType::PRINT:
          info_string += args_str;
          std::cout << info_string;
          break;
        default:
          break;
      }

      return args_str.size();
    }

    extern void info( const char *format, ... ) noexcept;
    extern void finfo( const char *format, ... ) noexcept;
    extern void error( const char *format, ... ) noexcept;
    extern void fatal( const char *format, ... ) noexcept;
    extern void warning( const char *format, ... ) noexcept;
    extern void print( const char *format, ... ) noexcept;

    extern void info( const char *format, ... ) noexcept;
    extern void finfo( const char *format, ... ) noexcept;
    extern void error( const char *format, ... ) noexcept;
    extern void fatal( const char *format, ... ) noexcept;
    extern void warning( const char *format, ... ) noexcept;
    extern void print( const char *format, ... ) noexcept;

  }    // namespace log

}    // namespace nile

