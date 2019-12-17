/* ================================================================================
$File: entity_manager.cc
$Date: 09 Oct 2019 $
$Revision: $
$Creator: Rostislav Orestis Stelmach
$Notice: $
================================================================================ */


#include "Nile/ecs/entity_manager.hh"
#include "Nile/core/assert.hh"

namespace nile {

  EntityManager::EntityManager() noexcept {
    // We reverse entity with ID 0 to represent a null entity
    for ( Entity entity = 1; entity < ecs::MAX_ENTITIES; ++entity ) {
      m_availableEntities.push( entity );
    }
  }

  [[nodiscard]] Entity EntityManager::createEntity() noexcept {

    ASSERT_M( m_livingEntities < ecs::MAX_ENTITIES, "To many entities in ecistance!" );

    // Take an ID from the front of the queue
    Entity id = m_availableEntities.front();
    m_availableEntities.pop();
    ++m_livingEntities;

    return id;
  }

  void EntityManager::freeEntity( Entity entity ) noexcept {

    ASSERT_M( entity < ecs::MAX_ENTITIES, "Entity out of range!" );

    // Invalidate the destroyed entity's signature
    m_signatures[ entity ].reset();

    // Put the freed ID at the back of the queue
    m_availableEntities.push( entity );
    --m_livingEntities;
  }

  void EntityManager::setSignature( Entity entity, Signature signature ) noexcept {
    ASSERT_M( entity < ecs::MAX_ENTITIES, " Entity out of range" );

    // Put the entity signature into the array
    m_signatures[ entity ] = signature;
  }

  [[nodiscard]] Signature EntityManager::getSignature( Entity entity ) const noexcept {
    ASSERT_M( entity < ecs::MAX_ENTITIES, "Entity out of range!" );
    return m_signatures[ entity ];
  }

  bool EntityManager::hasComponent( Entity entity, ComponentType component ) const noexcept {

    Signature signature;
    signature.set( component );

    return ( ( this->getSignature( entity ) & signature ) == signature );
  }

  usize EntityManager::getEntitiesCount() const noexcept {
    return m_livingEntities - 1;
  }

}    // namespace nile
