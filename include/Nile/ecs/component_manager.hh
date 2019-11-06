/* ================================================================================
$File: component_manager.hh
$Date: 09 Oct 2019 $
$Revision: $
$Creator: Rostislav Orestis Stelmach
$Notice: $
================================================================================ */

#pragma once

#include "Nile/core/assert.hh"
#include "Nile/core/types.hh"
#include "Nile/ecs/component_storage.hh"

#include <memory>
#include <unordered_map>

namespace nile {

  class ComponentManager {
  private:
    // Map from the string pointer to a component type
    std::unordered_map<const char *, ComponentType> m_componentsTypes;

    // Map from type string pointer to a component storage
    std::unordered_map<const char *, std::shared_ptr<IComponentStorage>> m_componentStorages;

    // the component type to e assigned to the next regisstered component - starting at 0
    ComponentType m_ntextComponentType {0};

    // Convenience function to get the statically casted pointer to the ComponentStorage of
    // type T
    template <typename T>
    std::shared_ptr<ComponentStorage<T>> getComponentStorage() noexcept {

      const char *typeName = typeid( T ).name();

      ASSERT_M( m_componentsTypes.find( typeName ) != m_componentsTypes.end(),
                "Component not registered" );

      return std::static_pointer_cast<ComponentStorage<T>>( m_componentStorages[ typeName ] );
    }

  public:
    template <typename T>
    void registerComponent() noexcept {
      const char *typeName = typeid( T ).name();

      ASSERT_M( m_componentsTypes.find( typeName ) == m_componentsTypes.end(),
                "Registering component more than once." );

      // Add this component type to the component map
      m_componentsTypes.insert( {typeName, m_ntextComponentType} );

      // Create a ComponentStorage pointer then add it to the component arrays map
      m_componentStorages.insert( {typeName, std::make_shared<ComponentStorage<T>>()} );

      // Increment the value so that the next component will be different
      ++m_ntextComponentType;
    }

    template <typename T>
    [[nodiscard]] ComponentType getComponentType() noexcept {
      const char *typeName = typeid( T ).name();

      ASSERT_M( m_componentsTypes.find( typeName ) != m_componentsTypes.end(),
                "Component not registered!" );

      // Return this component's type - used for creating signatures
      return m_componentsTypes[ typeName ];
    }

    template <typename T, typename... Args>
    void addComponent( Entity entity, Args &&... args ) noexcept {
      // Add a component to the array for an entity
      getComponentStorage<T>()->insertData( entity, std::forward<Args>( args )... );
    }

    template <typename T>
    void removeComponent( Entity entity ) noexcept {
      getComponentStorage<T>()->removeData( entity );
    }

    template <typename T>
    T &getComponent( Entity entity ) noexcept {
      return getComponentStorage<T>()->getData( entity );
    }

    void entityDestroyed( Entity entity ) {

      // Notify each component array that an entity has been destroyed
      // if it has a component for that entity, it will remove it
      for ( const auto &[ first, second ] : m_componentStorages ) {
        const auto &component = second;
        component->entityDestroyed( entity );
      }
    }

    u32 getComponentCount() const noexcept {
      return m_ntextComponentType;
    }
  };

}    // namespace nile
