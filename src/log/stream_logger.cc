#include "Nile/log/stream_logger.hh"

#include <cstdio>
#include <cstring>

namespace nile {

  void StreamLogger::printToStream( const char *msg, LogType type ) noexcept {
    printf( "%s", msg );
  }


}    // namespace nile
