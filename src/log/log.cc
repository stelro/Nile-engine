#include "Nile/log/log.hh"
#include "Nile/utils/string_utils.hh"

#include <GL/glew.h>
#include <cstdarg>

namespace nile {

  constexpr const char *CONSOLE_COLOR_RED = "\033[0;31m";
  constexpr const char *CONSOLE_COLOR_GREEN = "\033[0;32m";
  constexpr const char *CONSOLE_COLOR_YELLOW = "\033[0;33m";
  constexpr const char *CONSOLE_COLOR_MAGENTA = "\033[0;35m";
  constexpr const char *CONSOLE_COLOR_RESET = "\033[0m";


  log::MessageSignal log::on_message;

  std::string log::addTagToMessage( std::string &&str, LogType type ) noexcept {
    // @fix(stel): this need to be optimized some day, replace string
    // char literal or something else
    std::string final_str {};

    switch ( type ) {
      case LogType::NOTICE:
        final_str += "[   ";
        final_str += CONSOLE_COLOR_MAGENTA;
        final_str += "notice";
        final_str += CONSOLE_COLOR_RESET;
        final_str += "   ]: ";
        break;
      case LogType::ERROR:
        final_str += "[   ";
        final_str += CONSOLE_COLOR_RED;
        final_str += "error";
        final_str += CONSOLE_COLOR_RESET;
        final_str += "    ]: ";
        break;
      case LogType::WARNING:
        final_str += "[   ";
        final_str += CONSOLE_COLOR_YELLOW;
        final_str += "warning";
        final_str += CONSOLE_COLOR_RESET;
        final_str += "  ]: ";
        break;
        break;
      case LogType::FATAL:
        final_str += "[   ";
        final_str += CONSOLE_COLOR_RED;
        final_str += "fatal";
        final_str += CONSOLE_COLOR_RESET;
        final_str += "    ]: ";
        break;
      case LogType::NONE:
      case LogType::PRINT:
      case LogType::DISPLAY:
      default:
        break;
    }

    final_str += str;
    return final_str;
  }

  void log::error( const char *fmt, ... ) noexcept {
    va_list args;
    va_start( args, fmt );
    auto tmp = addTagToMessage( VaArgsToString( fmt, args ), LogType::ERROR );
    on_message.emit( tmp.c_str(), LogType::ERROR );
    va_end( args );
  }

  void log::warning( const char *fmt, ... ) noexcept {
    va_list args;
    va_start( args, fmt );
    auto tmp = addTagToMessage( VaArgsToString( fmt, args ), LogType::WARNING );
    on_message.emit( tmp.c_str(), LogType::WARNING );
    va_end( args );
  }

  void log::fatal( const char *fmt, ... ) noexcept {
    va_list args;
    va_start( args, fmt );
    auto tmp = addTagToMessage( VaArgsToString( fmt, args ), LogType::FATAL );
    on_message.emit( tmp.c_str(), LogType::FATAL );
    va_end( args );
  }

  void log::notice( const char *fmt, ... ) noexcept {
    va_list args;
    va_start( args, fmt );
    auto tmp = addTagToMessage( VaArgsToString( fmt, args ), LogType::NOTICE );
    on_message.emit( tmp.c_str(), LogType::NOTICE );
    va_end( args );
  }

  void log::print( const char *fmt, ... ) noexcept {
    va_list args;
    va_start( args, fmt );
    auto tmp = addTagToMessage( VaArgsToString( fmt, args ), LogType::PRINT );
    on_message.emit( tmp.c_str(), LogType::PRINT );
    va_end( args );
  }

  void log::console( const char *fmt, ... ) noexcept {
    va_list args;
    va_start( args, fmt );
    auto tmp = addTagToMessage( VaArgsToString( fmt, args ), LogType::DISPLAY );
    on_message.emit( tmp.c_str(), LogType::DISPLAY );
    va_end( args );
  }

  u32 log::checkGlErrors( const char *file, i32 line ) noexcept {
    GLenum errorCode;

    while ( ( errorCode = glGetError() ) != GL_NO_ERROR ) {
      std::string error;

      switch ( errorCode ) {
        case GL_INVALID_ENUM:
          error = "INVALID_ENUM";
          break;
        case GL_INVALID_VALUE:
          error = "INVALID_VALUE";
          break;
        case GL_INVALID_OPERATION:
          error = "INVALID_OPERATION";
          break;
        case GL_STACK_OVERFLOW:
          error = "STACK_OVERFLOW";
          break;
        case GL_STACK_UNDERFLOW:
          error = "STACK_UNDERFLOW";
          break;
        case GL_OUT_OF_MEMORY:
          error = "OUT_OF_MEMORY";
          break;
        case GL_INVALID_FRAMEBUFFER_OPERATION:
          error = "INVALID_FRAMEBUFFER_OPERATION";
          break;
      }

      log::error( "%s | %s (%d)\n", error.c_str(), file, line );
    }

    return errorCode;
  }

}    // namespace nile
