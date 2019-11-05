#include "Nile/log/log.hh"
#include "Nile/utils/string_utils.hh"

#include <cstdarg>

namespace nile {

  log::MessageSignal log::on_message;

  std::string log::addTagToMessage( std::string &&str, LogType type ) noexcept {
    // @fix(stel): this need to be optimized some day, replace string
    // char literal or something else
    std::string final_str {};

    switch ( type ) {
      case LogType::NOTICE:
        final_str += "[   notice  ]: ";
        break;
      case LogType::ERROR:
        final_str += "[   error   ]: ";
        break;
      case LogType::WARNING:
        final_str += "[  warning  ]: ";
        break;
      case LogType::FATAL:
        final_str += "[   fatal   ]: ";
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


}    // namespace nile
