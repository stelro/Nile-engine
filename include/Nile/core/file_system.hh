#pragma once

#include <string>
#include <vector>

namespace nile {

  class FileSystem {
  public:
    static std::string getSourceDir() noexcept;

    static std::string getPath( const std::string &path ) noexcept;

    static std::string getBinaryDir() noexcept;

    // @optimization: do we need handle IO/Async operations at reading file?!
    static std::vector<char> readFile( std::string_view fileName ) noexcept;

    static void sayHello() noexcept;
  };

}    // namespace nile
