/* ================================================================================
$File: ecs_coordinator.hh
$Date: 09 Oct 2019 $
$Revision: $
$Creator: Rostislav Orestis Stelmach
$Notice: $
================================================================================ */

#pragma once

#include "Nile/core/helper.hh"
#include "Nile/core/types.hh"
#include "Nile/ecs/component_manager.hh"
#include "Nile/ecs/ecs_system_manager.hh"
#include "Nile/ecs/entity_manager.hh"


#include <memory>

namespace nile {

  class Coordinator {
  private:
    std::unique_ptr<ComponentManager> m_componentManager;
    std::unique_ptr<EntityManager> m_entityManager;
    std::unique_ptr<EcsSystemManager> m_systemManager;

  public:
    Coordinator() = default;

    NILE_DISABLE_COPY( Coordinator )
    NILE_DISABLE_MOVE( Coordinator )

    void init() noexcept;

    [[nodiscard]] Entity createEntity() noexcept;

    void destroyEntity( Entity entity ) noexcept;

    template <typename T>
    void registerComponent() noexcept {
      m_componentManager->registerComponent<T>();
    }

    template <typename T, typename... Args>
    void addComponent( Entity entity, Args &&... args ) noexcept {
      m_componentManager->addComponent<T>( entity, std::forward<Args>( args )... );

      auto signature = m_entityManager->getSignature( entity );
      signature.set( m_componentManager->getComponentType<T>(), true );
      m_entityManager->setSignature( entity, signature );

      m_systemManager->entitySignatureChanged( entity, signature );
    }

    template <typename T>
    void removeComponent( Entity entity ) noexcept {
      m_componentManager->removeComponent<T>( entity );

      auto signature = m_entityManager->getSignature( entity );
      signature.set( m_componentManager->getComponentType<T>(), false );
      m_entityManager->setSignature( entity, signature );

      m_systemManager->entitySignatureChanged( entity, signature );
    }

    template <typename T>
    T &getComponent( Entity entity ) noexcept {
      return m_componentManager->getComponent<T>( entity );
    }

    template <typename T>
    ComponentType getComponentType() noexcept {
      return m_componentManager->getComponentType<T>();
    }

    // Relationship of entities
    bool attachTo( Entity parent, Entity child ) noexcept;

    void detach(Entity entity) noexcept;

    Entity getNext(Entity entity) noexcept;

    Entity getParent(Entity entity) noexcept;

    Entity getFirst(Entity entity) noexcept;

    template <typename T, typename... Args>
    std::shared_ptr<T> registerSystem( Args &&... args ) noexcept {
      return m_systemManager->registerSystem<T>( std::forward<Args>( args )... );
    }

    template <typename T>
    void setSystemSignature( Signature signature ) noexcept {
      m_systemManager->setSignature<T>( signature );
    }

    usize getEntitiesCount() const noexcept;

    u32 getComponentsCount() const noexcept;

    u32 getSystemsCount() const noexcept;

    // Wrapper method for ecs_system_manager createSystems, this should be called right
    // before update
    void createSystems() noexcept;

    void update( float dt ) noexcept;

    void render( float dt ) noexcept;
  };


}    // namespace nile
