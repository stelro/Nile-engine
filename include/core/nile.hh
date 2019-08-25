#pragma once

#include "logger.hh"
#include <cstdint>

namespace nile {

  // Types aliases
  using i8 = int8_t;
  using i16 = int16_t;
  using i32 = int32_t;
  using i64 = int64_t;
  using u8 = uint8_t;
  using u16 = uint16_t;
  using u32 = uint32_t;
  using u64 = uint32_t;
  using f32 = float;
  using f64 = double;
  using usize = std::size_t;
  using uptr = uintptr_t;

  // Helpers macros used to disable copy/move
  // constructors

#define NILE_DISABLE_COPY( Class )                                                                 \
  Class( const Class & ) = delete;                                                                 \
  Class &operator=( const Class & ) = delete;

#define NILE_DISABLE_MOVE( Class )                                                                 \
  Class( const Class && ) = delete;                                                                \
  Class &operator=( const Class && ) = delete;

#define ASSERT_M( condition, msg )                                                                 \
  \ do {                                                                                           \
    \ if ( !( condition ) ) {                                                                      \
      \ fn::log::fatal( "%s(%d): Assertion failed with error - %s\n", __FILE__, __LINE__, msg );   \
      \                                                                                            \
    }                                                                                              \
  }                                                                                                \
  while ( 0 )                                                                                      \
    ;

#define ASSERT( condition )                                                                        \
  do {                                                                                             \
    if ( !( condition ) ) {                                                                        \
      fn::log::fatal( "%s(%d): Assertion failed\n", __FILE__, __LINE__ );                          \
    }                                                                                              \
  } while ( 0 );

}    // namespace nile
