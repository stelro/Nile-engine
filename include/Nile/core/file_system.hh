#pragma once

#include "Nile/core/error_code.hh"

#include <string>
#include <variant>
#include <vector>

namespace nile {

  class FileSystem {
  public:
    static std::string getSourceDir() noexcept;

    static std::string getPath( const std::string &path ) noexcept;

    static std::string getBinaryDir() noexcept;

    // @optimization: do we need handle IO/Async operations at reading file?!
    // returns variant, may contain payload or errorcode
    static std::variant< ErrorCode,std::vector<char>>
    readFile( std::string_view fileName ) noexcept;
  };

}    // namespace nile
