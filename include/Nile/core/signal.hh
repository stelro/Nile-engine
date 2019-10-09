/* ================================================================================
$File: signal.hh
$Date: $
$Revision: $
$Creator: Rostislav Orestis Stelmach
$Notice: $
================================================================================ */

#pragma once

#include "Nile/core/helper.hh"

#include <functional>
#include <list>
#include <memory>

// Let the users know that one class who uses 
// singals, is observable, and the one who has listener is
// observer
#define OBSERVABLE
#define OBSERVER

namespace nile {

  template <typename... Args>
  class Signal {
  private:
    std::shared_ptr<std::list<std::function<void( Args... )>>> m_signals;

  public:
    using slot = std::function<void( Args... )>;
    using listener_list = std::list<slot>;

    Signal()
        : m_signals( std::make_shared<listener_list>() ) {}

    ~Signal() {}


    struct Listener {
      std::weak_ptr<listener_list> signal;
      typename listener_list::iterator iterator;

      Listener() {
        // Empty constroctur
      }

      Listener( Signal &t_signal, slot t_slot ) {
        observe( t_signal, t_slot );
      }

      Listener( Listener &&rhs ) {
        this->signal = rhs.signal;
        iterator = rhs.iterator;
        rhs.signal.reset();
      }

      Listener &operator=( Listener &&rhs ) {
        if ( *this != &rhs ) {
          reset();
          this->signal = rhs.signal;
          iterator = rhs.iterator;
          rhs.signal.reset();
        }
        return *this;
      }

      NILE_DISABLE_COPY( Listener )

      void connect( Signal &t_signal, slot t_slot ) {
        reset();
        this->signal = t_signal.m_signals;
        iterator = t_signal.m_signals->insert( t_signal.m_signals->end(), t_slot );
      }

      void reset() {
        if ( !signal.expired() )
          signal.lock()->erase( iterator );
        signal.reset();
      }
    };

    void emit( Args... args ) noexcept {
      for ( auto &f : *m_signals ) {
        f( args... );
      }
    }

    Listener connect( slot t_slot ) {
      return Listener( *this, t_slot );
    }
  };

}    // namespace nile
