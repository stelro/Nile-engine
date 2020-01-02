#pragma once

#include <string>

namespace nile {

  class FileSystem {
  public:
    static std::string getSourceDir() noexcept;

    static std::string getPath( const std::string &path ) noexcept;

    static std::string getBinaryDir() noexcept;

  };

}    // namespace nile
