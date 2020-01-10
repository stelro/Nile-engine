#pragma once

#include "Nile/core/assert.hh"
#include <string>

namespace nile {

  // @brief: various error codes used by functions whose returns std::variant
  // with error code attached and the payload

  enum class ErrorCode {
    NONE = 0,    // success
    FILE_NOT_FOUND = 16,
    FILE_IS_EMPTY = 17
  };

  [[nodiscard]]
  std::string errorToString( ErrorCode code ) noexcept;
}    // namespace nile
