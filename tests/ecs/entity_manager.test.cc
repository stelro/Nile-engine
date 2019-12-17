#include <Nile/ecs/component_manager.hh>
#include <Nile/ecs/ecs_coordinator.hh>
#include <Nile/ecs/entity_manager.hh>
#include <catch.hpp>
#include <glm/glm.hpp>
#include <iostream>
#include <vector>

using nile::ComponentManager;
using nile::Entity;
using nile::EntityManager;
using nile::Signature;

namespace {

  // some dumb components
  struct transform {
    glm::vec3 position;
  };

  struct renderable {
    glm::vec3 color;
  };

  struct physics {
    float velocity;
  };

}    // namespace

TEST_CASE( "EntityManager::createEntity,", "[EntityManager]" ) {

  SECTION( "The first created entity sould have ID equal to 1" ) {
    EntityManager manager;
    auto entity = manager.createEntity();
    REQUIRE( entity == 1 );
  }

  {
    EntityManager manager;

    SECTION( "Creating new entities should change entities count" ) {
      constexpr std::uint32_t entitiesSize = 10;
      std::vector<Entity> entities;


      for ( uint32_t i = 0; i < entitiesSize; i++ ) {
        entities.emplace_back( manager.createEntity() );
      }

      REQUIRE( manager.getEntitiesCount() == entitiesSize );
      REQUIRE( manager.getEntitiesCount() == entities.size() );
    }

    SECTION( "Freeing some entities should decrease the entities count" ) {
      constexpr std::uint32_t entitiesSize = 10;
      constexpr std::uint32_t entitiesToRemove = 3;

      std::vector<Entity> entities;

      for ( uint32_t i = 0; i < entitiesSize; i++ ) {
        entities.emplace_back( manager.createEntity() );
      }

      for ( uint32_t i = 0; i < entitiesToRemove; i++ ) {
        manager.freeEntity( entities.back() );
        entities.pop_back();
      }

      REQUIRE( manager.getEntitiesCount() == ( entitiesSize - entitiesToRemove ) );
    }
  }

  SECTION( "Entities should have the right signature" ) {

    ComponentManager componentManager;
    EntityManager manager;

    // Register some components
    componentManager.registerComponent<transform>();
    componentManager.registerComponent<renderable>();
    componentManager.registerComponent<physics>();

    // Create a signature that represent a speciall system
    // consist of those components
    Signature signature;
    signature.set( componentManager.getComponentType<transform>() );
    signature.set( componentManager.getComponentType<renderable>() );
    signature.set( componentManager.getComponentType<physics>() );

    auto entity = manager.createEntity();

    manager.setSignature( entity, signature );

    REQUIRE( signature == manager.getSignature( entity ) );
  }

  SECTION( "Check if entity has a specific component attached to it" ) {

    ComponentManager componentManager;
    EntityManager manager;

    // Register some components
    componentManager.registerComponent<transform>();
    componentManager.registerComponent<renderable>();
    componentManager.registerComponent<physics>();

    // Create a signature that represent a speciall system
    // consist of those components
    Signature signature;
    signature.set( componentManager.getComponentType<transform>() );
    signature.set( componentManager.getComponentType<renderable>() );

    auto entity = manager.createEntity();

    manager.setSignature( entity, signature );

    REQUIRE( manager.hasComponent( entity, componentManager.getComponentType<transform>() ) );
    REQUIRE( !manager.hasComponent( entity, componentManager.getComponentType<physics>() ) );
  }
}
