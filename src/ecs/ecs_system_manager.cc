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
    // m_entities is a set so no check needed
    for ( const auto &[ first, second ] : m_systems ) {
      const auto &system = second;
      system->m_entities.erase( entity );
    }
  }

  void EcsSystemManager::entitySignatureChanged( Entity entity,
                                                 Signature entitySignature ) noexcept {

    // Notify each system that an entity's signature has changed
    for ( const auto &[ first, second ] : m_systems ) {

      const auto &type = first;
      const auto &system = second;
      const auto &systemSignature = m_signatures[ type ];

      // Entity signature matches sytem signature - insert into set
      if ( ( entitySignature & systemSignature ) == systemSignature ) {
        system->m_entities.insert( entity );
      } else {
        // Entity signature does not match system signature - erase from the set
        system->m_entities.erase( entity );
      }
    }
  }

}    // namespace nile
