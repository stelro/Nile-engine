/* ================================================================================
$File: ecs_system_manager.cc
$Date: 09 Oct 2019 $
$Revision: $
$Creator: Rostislav Orestis Stelmach
$Notice: $
================================================================================ */

#include "Nile/ecs/ecs_system_manager.hh"

namespace nile {

  void EcsSystemManager::entityDestroyed( Entity entity ) noexcept {
    // Erase a destroyed entity from all systems lists
    // entiteis_ is a set so no check needed
    for ( const auto &[ first, second ] : systems_ ) {
      const auto &system = second;
      system->entities_.erase( entity );
    }
  }

  void EcsSystemManager::entitySignatureChanged( Entity entity,
                                                 Signature entitySignature ) noexcept {

    // Notify each system that an entity's signature has changed
    for ( const auto &[ first, second ] : systems_ ) {

      const auto &type = first;
      const auto &system = second;
      const auto &systemSignature = signatures_[ type ];

      // Entity signature matches sytem signature - insert into set
      if ( ( entitySignature & systemSignature ) == systemSignature ) {
        system->entities_.insert( entity );
      } else {
        // Entity signature does not match system signature - erase from the set
        system->entities_.erase( entity );
      }
    }
  }

  void EcsSystemManager::createSystems() noexcept {
    for ( const auto &create_h : create_handles_ ) {
      create_h();
    }
  }

  void EcsSystemManager::update( float dt ) noexcept {
    for ( const auto &update_h : update_handles_ ) {
      update_h( dt );
    }
  }

  void EcsSystemManager::render( float dt ) noexcept {
    for ( const auto &render_h : render_handles_ ) {
      render_h( dt );
    }
  }

  u32 EcsSystemManager::getSystemsCount() const noexcept {
    return systems_.size();
  }

}    // namespace nile
