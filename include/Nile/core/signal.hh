/* ================================================================================
$File: signal.hh
$Date: $
$Revision: $
$Creator: Rostislav Orestis Stelmach
$Notice: $
================================================================================ */

#pragma once

#include "Nile/core/types.hh"

#include <functional>
#include <unordered_map>

// Signals-slots design pattern, pretty much like the one used by Boost
// and Qt framework. It basically implements the observer pattern.
//
// A signal object may call multiple slots with the same signature.
// We can connect functions to the signal or class methods, which will be
// called when the emit() method on the signal object is invoked.
//
// Simple usage
//
// class Player {
// public:
//   Signal<bool> on_jump;
// };
//
// class Wall {
// public:
//   void break() const {
//     std::cout << "Wall is broken now" << '\n';
//  }
// };
//
// int main() {
//
//  Player player;
//  Wall wall;
//
//  player.on_jump.connect(&wall, &Message::break);
//  player.on_jump.emmit(true);
//
// }

namespace nile {

  template <typename... Args>
  class Signal {
  private:
    using Slot = std::function<void( Args... )>;

    mutable std::unordered_map<u32, Slot> m_slots;
    mutable u32 m_currentSlotId = 0;

  public:
    Signal() noexcept
        : m_currentSlotId( 0 ) {}

    Signal( const Signal &rhs )
        : m_currentSlotId( 0 ) {}

    // Create new Signal
    Signal &operator=( const Signal &rhs ) noexcept {
      m_currentSlotId = 0;
      this->deattachAll();
      return *this;
    }

    // attach a slot of type std::function to this signal.
    u32 connect( const Slot &slot ) const noexcept {
      m_slots.insert( std::make_pair( ++m_currentSlotId, slot ) );
      return m_currentSlotId;
    }

    // attach a member method to this signal
    template <typename T>
    u32 connect( T *instance, void ( T::*function )( Args... ) ) {
      return connect( [=]( Args... args ) { ( instance->*function )( args... ); } );
    }

    // attach a const member method to this signal
    template <typename T>
    u32 connect( T *instance, void ( T::*function )( Args... ) const ) {
      return connect( [=]( Args... args ) { ( instance->*function )( args... ); } );
    }

    // deattach a connected slot by id
    void deattach( u32 id ) noexcept {
      m_slots.erase( id );
    }

    // deattach all slots from current signal
    void deattachAll() noexcept {
      m_slots.clear();
    }

    void emit( Args... args ) {
      for ( const auto &i : m_slots ) {
        i.second( std::forward<Args>( args )... );
      }
    }
  };

}    // namespace nile
