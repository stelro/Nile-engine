/* ================================================================================
$File: assert.hh
$Date: 09 Oct 2019 $
$Revision: $
$Creator: Rostislav Orestis Stelmach
$Notice: $
================================================================================ */

#pragma once

#include <cstdio>
#include <cstdlib>

#define STR( x ) #x

#define ASSERT_M( condition, msg )                                                                 \
  do {                                                                                             \
    if ( !( condition ) ) {                                                                        \
      printf( "\n\033[0;31mAssertion Failed ->\033[0m [ %s ]\n - function: %s\n - file: %s\n - "   \
              "line: %d\n - reason: %s\n\n",                                                                      \
              STR( condition ), __PRETTY_FUNCTION__, __FILE__, __LINE__, msg );                         \
      std::abort();                                                                                \
    }                                                                                              \
  } while ( 0 );

#define ASSERT( condition )                                                                        \
  do {                                                                                             \
    if ( !( condition ) ) {                                                                        \
      printf( "\n\033[0;31mAssertion Failed ->\033[0m [ %s ]\n - function: %s\n - file: %s\n - "   \
              "line: %d\n\n",                                                                      \
              STR( condition ), __PRETTY_FUNCTION__, __FILE__, __LINE__ );                         \
      std::abort();                                                                                \
    }                                                                                              \
  } while ( 0 );

