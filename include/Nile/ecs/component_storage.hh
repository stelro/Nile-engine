/* ================================================================================
$File: component_manager.hh
$Date: 09 Oct 2019 $
$Revision: $
$Creator: Rostislav Orestis Stelmach
$Notice: $
================================================================================ */

#pragma once

#include "Nile/core/types.hh"
#include "Nile/core/assert.hh"

#include <array>
#include <unordered_map>

namespace nile {

  class IComponentStorage {
  public:
    virtual ~IComponentStorage() = default;
    virtual void entityDestroyed( Entity entity ) noexcept = 0;
  };

  template <typename T>
  class ComponentStorage : public IComponentStorage {
  private:
    // The packed array of components of generic type T
    // set to a specified maximum ammount, matching the maxiumum
    // number of entities allowed to exist simultaneaously, so that
    // each entity has a unique spot.
    std::array<T, ecs::MAX_ENTITIES> m_componentsStorage;

    // Map from an entity ID to an array index.
    std::unordered_map<Entity, usize> m_entityToIndexMap;

    // Map from an array index to an entity ID
    std::unordered_map<usize, Entity> m_indexToEntityMap;

    // Total size of valid entires in the array
    usize m_size;

  public:
    void insertData( Entity entity, T component ) noexcept {
      ASSERT_M( m_entityToIndexMap.find( entity ) == m_entityToIndexMap.end(),
                "Component added to same entity more than once!" );

      usize newIndex = m_size;
      m_entityToIndexMap[ entity ] = newIndex;
      m_indexToEntityMap[ newIndex ] = entity;
      m_componentsStorage[ newIndex ] = component;
      ++m_size;
    }

    void removeData( Entity entity ) noexcept {
      ASSERT_M( m_entityToIndexMap.find( entity ) != m_entityToIndexMap.end(),
                "Removing non-existing component!" );

      // Copy the element at the end into deleted element's to maintain density
      usize indexOfRemovedEntity = m_entityToIndexMap[ entity ];
      usize indexOfLastElement = m_size - 1;
      m_componentsStorage[ indexOfRemovedEntity ] = m_componentsStorage[ indexOfLastElement ];

      // Update the map to point to moved spot
      Entity entityOfLastElement = m_indexToEntityMap[ indexOfLastElement ];
      m_entityToIndexMap[ entityOfLastElement ] = indexOfRemovedEntity;
      m_indexToEntityMap[ indexOfRemovedEntity ] = entityOfLastElement;

      m_entityToIndexMap.erase( entity );
      m_indexToEntityMap.erase( indexOfLastElement );

      --m_size;
    }

    [[nodiscard]] T &getData( Entity entity ) noexcept {

      ASSERT_M( m_entityToIndexMap.find( entity ) != m_entityToIndexMap.end(),
                "Retriving non-existing component!" );

      // Return a reference to the entity's component
      return m_componentsStorage[ m_entityToIndexMap[ entity ] ];
    }

    void entityDestroyed( Entity entity ) noexcept override {

      if ( m_entityToIndexMap.find( entity ) != m_entityToIndexMap.end() ) {
        // Remove the entity's component if it existied
        removeData( entity );
      }
    }
  };

}    // namespace nile
