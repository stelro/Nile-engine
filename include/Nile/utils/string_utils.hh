#pragma once

#include <cstdarg>
#include <string>
#include <vector>

namespace nile {

  template <typename... Args>
  std::string VaArgsToString( const std::string &str, va_list args ) noexcept {
    va_list args_copy;
    va_copy( args_copy, args );

    auto size = vsnprintf( nullptr, 0, str.c_str(), args ) + 1;

    std::string tmp_str;
    tmp_str.resize( static_cast<unsigned long>( size ) );
    vsnprintf( &tmp_str[ 0 ], static_cast<size_t>( size ), str.c_str(), args_copy );
    tmp_str.pop_back();    // Remove trialign character

    return tmp_str;
  }

  template <typename... Args>
  std::vector<char> VaArgsToVector( const std::string &str, va_list args ) noexcept {
    va_list args_copy;
    va_copy( args_copy, args );

    auto size = vsnprintf( nullptr, 0, str.c_str(), args ) + 1;

    std::vector<char> tmp_vec;

    tmp_vec.resize( static_cast<unsigned long>( size ) );
    vsnprintf( &tmp_vec[ 0 ], static_cast<size_t>( size ), str.c_str(), args_copy );
    tmp_vec.pop_back();    // Remove trialign character

    return tmp_vec;
  }


}    // namespace nile
