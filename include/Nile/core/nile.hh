/* ================================================================================
$File: nile.hh
$Date: $
$Revision: $
$Creator: Rostislav Orestis Stelmach
$Notice: $
================================================================================ */

#pragma once

#include "Nile/core/logger.hh"
#include <cstdint>


// Helpers macros used to disable copy/move
// constructors

#define NILE_DISABLE_COPY( Class )                                                                 \
  Class( const Class & ) = delete;                                                                 \
  Class &operator=( const Class & ) = delete;

#define NILE_DISABLE_MOVE( Class )                                                                 \
  Class( const Class && ) = delete;                                                                \
  Class &operator=( const Class && ) = delete;


