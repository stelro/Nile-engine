/* ================================================================================
$File: ecs_coordinator.cc
$Date: 09 Oct 2019 $
$Revision: $
$Creator: Rostislav Orestis Stelmach
$Notice: $
================================================================================ */

#include "Nile/ecs/ecs_coordinator.hh"


namespace nile {

  void Coordinator::init() noexcept {

    m_componentManager = std::make_unique<ComponentManager>();
    m_entityManager = std::make_unique<EntityManager>();
    m_systemManager = std::make_unique<EcsSystemManager>();
  }

  [[nodiscard]] Entity Coordinator::createEntity() noexcept {
    return m_entityManager->createEntity();
  }

  void Coordinator::destroyEntity( Entity entity ) noexcept {
    m_entityManager->freeEntity( entity );
    m_componentManager->entityDestroyed( entity );
    m_systemManager->entityDestroyed( entity );
  }


}    // namespace nile
