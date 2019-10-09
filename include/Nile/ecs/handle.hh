/* ================================================================================
$File: handle.hh
$Date: 09 Oct 2019 $
$Revision: $
$Creator: Rostislav Orestis Stelmach
$Notice: $
================================================================================ */

#pragma once

#include <functional>    // hash

namespace nile {

  class Handle {
  public:
    static constexpr int INDEX_BITS = 16;
    static constexpr int COUNTER_BITS = 16;

    unsigned index : INDEX_BITS;
    unsigned counter : COUNTER_BITS;

    constexpr Handle() noexcept
        : index( 0 )
        , counter( 0 ) {}
    constexpr Handle( unsigned t_index, unsigned t_counter ) noexcept
        : index( t_index )
        , counter( t_counter ) {}

    constexpr inline bool operator==( const Handle &rhs ) const noexcept {
      return index == rhs.index && counter == rhs.counter;
    }

    constexpr inline bool operator!=( const Handle &rhs ) const noexcept {
      return !( *this == rhs );
    }

    constexpr bool isValid() const noexcept {
      return !( index == 0 && counter == 0 );
    }

    constexpr operator bool() const noexcept {
      return isValid();
    }

    constexpr unsigned hash() const noexcept {
      return counter << INDEX_BITS | index;
    }
  };

}    // namespace nile

namespace std {

  template <>
  struct hash<nile::Handle> {
    size_t operator()( const nile::Handle &s ) const noexcept {
      return s.hash();
    }
  };

}    // namespace std
