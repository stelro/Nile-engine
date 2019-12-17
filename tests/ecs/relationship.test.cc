#include <Nile/ecs/components/relationship.hh>
#include <Nile/ecs/ecs_coordinator.hh>
#include <catch.hpp>

using nile::Coordinator;
using nile::Relationship;

TEST_CASE( "Coordinator::attachTo", "[Coordinator]" ) {

  Coordinator coordinator;
  coordinator.init();
  coordinator.registerComponent<Relationship>();

  auto model_entity = coordinator.createEntity();
  auto body_entity = coordinator.createEntity();
  auto head_entity = coordinator.createEntity();
  auto left_arm_entity = coordinator.createEntity();
  auto right_arm_entity = coordinator.createEntity();
  auto right_leg_entity = coordinator.createEntity();
  auto left_leg_entity = coordinator.createEntity();

  coordinator.addComponent<Relationship>( model_entity, Relationship() );
  coordinator.addComponent<Relationship>( body_entity, Relationship() );
  coordinator.addComponent<Relationship>( head_entity, Relationship() );
  coordinator.addComponent<Relationship>( left_arm_entity, Relationship() );
  coordinator.addComponent<Relationship>( right_arm_entity, Relationship() );
  coordinator.addComponent<Relationship>( right_leg_entity, Relationship() );
  coordinator.addComponent<Relationship>( left_leg_entity, Relationship() );

  SECTION( "Entities should have hierarchy" ) {

    // Pretend that we are loading a 3D model character
    // consist of multiple meshes that represents various
    // kinds of body parts, like legs, body, arms, etc..
    // the model_entity is the main parent entity that represents
    // the model as a whole.

    coordinator.attachTo( model_entity, body_entity );
    coordinator.attachTo( model_entity, head_entity );
    coordinator.attachTo( model_entity, left_arm_entity );
    coordinator.attachTo( model_entity, right_arm_entity );

    SECTION( "Entities in hierarchy have next child" ) {
      REQUIRE( coordinator.getComponent<Relationship>( head_entity ).next == body_entity );
    }

    SECTION( "Entities in hierarchy have prev child" ) {
      REQUIRE( coordinator.getComponent<Relationship>( head_entity ).prev == left_arm_entity );
    }

    SECTION( "Entities in hierarchy have parent" ) {
      REQUIRE( coordinator.getComponent<Relationship>( head_entity ).parent == model_entity );
    }

    SECTION( "Parent have first child" ) {
      REQUIRE( coordinator.getComponent<Relationship>( model_entity ).firstChild == right_arm_entity );
    }


    auto curr = coordinator.getFirst( model_entity );

    std::uint32_t counter = 0;

    while ( curr != nile::ecs::null ) {
      counter++;
      curr = coordinator.getNext( curr );
    }

    REQUIRE( counter == 4 );
  }

  SECTION( "Entities can detach from the list" ) {

    coordinator.attachTo( model_entity, body_entity );
    coordinator.attachTo( model_entity, head_entity );
    coordinator.attachTo( model_entity, left_arm_entity );
    coordinator.attachTo( model_entity, right_arm_entity );

    coordinator.detach( right_arm_entity );
    coordinator.detach( body_entity );

    auto curr = coordinator.getFirst( model_entity );
    std::uint32_t counter = 0;

    while ( curr != nile::ecs::null ) {
      counter++;
      curr = coordinator.getNext( curr );
    }

    REQUIRE( counter == 2 );
  }
}

