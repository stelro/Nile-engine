#pragma once

#include <type_traits>

namespace nile {

  // Some SFINAE to detect specific methods in the ecs systems
  // ( damn, I love c++ and template metaprogramming )
  namespace detail {

    template <typename C>
    constexpr decltype( std::declval<C>().update( std::declval<float>() ), bool() )
    has_update( int ) {
      return true;
    }

    template <typename C>
    constexpr bool has_update( ... ) {
      return false;
    }

    template <typename C>
    constexpr decltype( std::declval<C>().render( std::declval<float>() ), bool() )
    has_render( int ) {
      return true;
    }

    template <typename C>
    constexpr bool has_render( ... ) {
      return false;
    }

    template <typename C>
    constexpr decltype( std::declval<C>().create(), bool() ) has_create( int ) {
      return true;
    }

    template <typename C>
    constexpr bool has_create( ... ) {
      return false;
    }

    template <typename C>
    constexpr decltype( std::declval<C>().create(), bool() ) has_destroy( int ) {
      return true;
    }

    template <typename C>
    constexpr bool has_destroy( ... ) {
      return false;
    }


  }    // namespace detail

  template <typename T>
  constexpr bool ecs_has_update = detail::has_update<T>( 0 );

  template <typename T>
  constexpr bool ecs_has_render = detail::has_render<T>( 0 );

  template <typename T>
  constexpr bool ecs_has_create = detail::has_create<T>( 0 );

  template <typename T>
  constexpr bool ecs_has_destroy = detail::has_destroy<T>( 0 );


}    // namespace nile
