/* ================================================================================
$File: assert.hh
$Date: 09 Oct 2019 $
$Revision: $
$Creator: Rostislav Orestis Stelmach
$Notice: $
================================================================================ */

#pragma once

#include <cstdio>

#define ASSERT_M( condition, msg )                                                                 \
  do {                                                                                             \
    if ( !( condition ) ) {                                                                        \
      printf( "%s(%d): Assertion failed with error - %s\n", __FILE__, __LINE__, msg );   \
    }                                                                                              \
  } while ( 0 );

#define ASSERT( condition )                                                                        \
  do {                                                                                             \
    if ( !( condition ) ) {                                                                        \
      printf( "%s(%d): Assertion failed\n", __FILE__, __LINE__ );                        \
    }                                                                                              \
  } while ( 0 );

