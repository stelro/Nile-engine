#include "Nile/core/file_system.hh"
#include "Nile/core/assert.hh"
#include "Nile/core/types.hh"

#include "Nile/log/log.hh"

#include "project_config.h"

#include <fstream>

namespace nile {

  std::string FileSystem::getSourceDir() noexcept {
    return ( logl_root != nullptr ) ? logl_root : " ";
  }

  std::string FileSystem::getPath( const std::string &path ) noexcept {
    return getSourceDir() + '/' + path;
  }

  std::string FileSystem::getBinaryDir() noexcept {
    return ( binary_root != nullptr ) ? binary_root : " ";
  }

  std::vector<char> FileSystem::readFile( std::string_view fileName ) noexcept {

    std::ifstream file( fileName.data(), std::ios::ate | std::ios::binary );

    ASSERT_M( file.is_open(), "Failed to open file\n" );

    auto file_size = static_cast<size_t>( file.tellg() );
    std::vector<char> buffer( file_size );

    file.seekg( 0 );
    file.read( buffer.data(), file_size );

    file.close();

    return buffer;
  }

}    // namespace nile
