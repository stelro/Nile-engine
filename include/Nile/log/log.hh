#pragma once

#include "Nile/core/signal.hh"
#include "Nile/log/log_type.hh"

#define glCheckError() log::checkGlErrors( __FILE__, __LINE__ )

namespace nile {

    class log {
  private:
    static std::string addTagToMessage( std::string &&str, LogType type ) noexcept;

  public:
    using MessageSignal = Signal<const char *, LogType>;
    static MessageSignal on_message;

    // errors will be printed out to stderr/file/editor console
    static void error( const char *fmt, ... ) noexcept;

    // warning will be printed out to stdout/file/editor console
    static void warning( const char *fmt, ... ) noexcept;

    // fatal will be printed out to stdout/file
    static void fatal( const char *fmt, ... ) noexcept;

    // notice will be printed out to stdout/file/editor console
    static void notice( const char *fmt, ... ) noexcept;

    // print will be printed out to stdout/file/editor console
    static void print( const char *fmt, ... ) noexcept;

    // console will be printed out to editor console only
    static void console( const char *fmt, ... ) noexcept;

    static u32 checkGlErrors( const char *file, i32 line ) noexcept;
  };

}    // namespace nile
