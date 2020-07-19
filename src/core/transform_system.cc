/* ================================================================================
$File: transform_system.cc
$Date: 19 Jul 2020 $
$Revision: $
$Creator: Rostislav Orestis Stelmach
$Notice: $
================================================================================ */

#include "Nile/core/transform_system.hh"
#include "Nile/ecs/components/relationship.hh"
#include "Nile/ecs/components/transform.hh"
#include "Nile/ecs/ecs_coordinator.hh"

#include <spdlog/spdlog.h>

namespace nile {

  TransformSystem::TransformSystem( const std::shared_ptr<Coordinator> &coordinator )
      : ecs_coordinator_( coordinator ) {}

  void TransformSystem::create() {
    spdlog::info(
        "ECS TransformSystem has been registered to ECS manager and created successfully." );
  }

  void TransformSystem::update() {

    for ( const auto &entity : entities_ ) {
      [[maybe_unused]] auto &relationship = ecs_coordinator_->getComponent<Relationship>( entity );
      [[maybe_unused]] auto &transform = ecs_coordinator_->getComponent<Transform>( entity );
    }
  }

  void TransformSystem::render() {}


}    // namespace nile

