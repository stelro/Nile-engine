#include "Nile/core/error_code.hh"

namespace nile {

  [[nodiscard]] std::string errorToString( ErrorCode code ) noexcept {
    switch ( code ) {
      case ErrorCode::NONE:
        return "NONE";
      case ErrorCode::FILE_NOT_FOUND:
        return "FILE_NOT_FOUND";
      case ErrorCode::FILE_IS_EMPTY:
        return "FILE_IS_EMPTY";
      default:
        return "UNKNOWN_ERROR";
    }
  }

}    // namespace nile
