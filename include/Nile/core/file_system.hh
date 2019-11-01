#pragma once

#include <string>

namespace nile {

  class FileSystem {
  public:
    static std::string getRoot() noexcept;

    static std::string getPath( const std::string &path ) noexcept;
  };

}    // namespace nile