/* ================================================================================
$File: logger.cc
$Date: $
$Revision: $
$Creator: Rostislav Orestis Stelmach
$Notice: $
================================================================================ */

#include "Nile/core/logger_deprecated.hh"

namespace nile {

  namespace log {

    void info( const char *format, ... ) noexcept {

      va_list args;
      va_start( args, format );
      process_log( LogType::INFO, format, args );
      va_end( args );
    }

    void finfo( [[maybe_unused]]const char *format, ... ) noexcept {
#ifdef NDEBUG
      return;
#endif
      // Empty, will be used to log to file
    }

    void error( const char *format, ... ) noexcept {
#ifdef NDEBUG
      return;
#endif
      va_list args;
      va_start( args, format );
      process_log( LogType::ERROR, format, args );
      va_end( args );
    }

    void warning( const char *format, ... ) noexcept {
#ifdef NDEBUG
      return;
#endif
      va_list args;
      va_start( args, format );
      process_log( LogType::WARNING, format, args );
      va_end( args );
    }

    void fatal( const char *format, ... ) noexcept {

      va_list args;
      va_start( args, format );
      process_log( LogType::FATAL, format, args );
      va_end( args );
      // Force exit application
      exit( EXIT_FAILURE );
    }

    void print( const char *format, ... ) noexcept {

      va_list args;
      va_start( args, format );
      process_log( LogType::PRINT, format, args );
      va_end( args );
    }


  }    // namespace log

}    // namespace fn
