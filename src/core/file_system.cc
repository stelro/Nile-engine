#include "Nile/core/file_system.hh"

#include "project_config.h"

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

}    // namespace nile
