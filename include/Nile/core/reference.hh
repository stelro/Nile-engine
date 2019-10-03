/* ================================================================================
$File: reference.hh
$Date: $
$Revision: $
$Creator: Rostislav Orestis Stelmach
$Notice: $
================================================================================ */

#pragma once

#include "Nile/core/nile.hh"

namespace nile {

  class Reference {
  private:
    u32 m_ref;

  public:
    Reference()
        : m_ref( 1 ) {}

    void inc() {
      m_ref++;
    }

    void dec() {
      --m_ref;
    }

    u32 getRefCount() const {
      return m_ref;
    }
  };

}    // namespace nile
