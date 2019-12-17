#include "Nile/core/types.hh"
#include <catch.hpp>
#include <type_traits>

using nile::ComponentType;
using nile::Entity;

TEST_CASE( "Entity", "[Entity]" ) {

  constexpr ComponentType maxComponents = 64;

  SECTION( "entity is type of std::uint32_t" ) {
    REQUIRE( std::is_same<std::uint32_t, Entity>::value );
  }

  SECTION( "ComponentType is type of std::uint8t" ) {
    REQUIRE( std::is_same<std::uint8_t, ComponentType>::value );
  }

  SECTION( "ecs::null is equals to 0" ) {
    REQUIRE( nile::ecs::null == 0 );
  }

  SECTION( "MAX_ENTITIES are euqal to 32768" ) {
    REQUIRE( nile::ecs::MAX_ENTITIES == 32768 );
  }

  SECTION( "MAX_COMPONENTS are equal to 64" ) {
    REQUIRE( nile::ecs::MAX_COMPONENTS == maxComponents );
  }
}
