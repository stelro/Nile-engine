#include "Nile/log/stream_logger.hh"

#include <cstdio>
#include <cstring>

namespace nile {

  void StreamLogger::printToStream( const char *msg, LogType type ) noexcept {
    if ( type == ( type & ( LogType::NONE | LogType::ERROR | LogType::PRINT | LogType::FATAL ) ) )
      printf( "%s", msg );
  }


}    // namespace nile
