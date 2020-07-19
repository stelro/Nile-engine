/* ================================================================================
$File: ecs_coordinator.cc
$Date: 09 Oct 2019 $
$Revision: $
$Creator: Rostislav Orestis Stelmach
$Notice: $
================================================================================ */

#include "Nile/ecs/ecs_coordinator.hh"
#include "Nile/ecs/components/relationship.hh"
#include "spdlog/spdlog.h"

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

  void Coordinator::createSystems() noexcept {
    m_systemManager->createSystems();
  }

  void Coordinator::update( float dt ) noexcept {
    m_systemManager->update( dt );
  }

  void Coordinator::render( float dt ) noexcept {
    m_systemManager->render( dt );
  }

  usize Coordinator::getEntitiesCount() const noexcept {
    return m_entityManager->getEntitiesCount();
  }

  u32 Coordinator::getComponentsCount() const noexcept {
    return m_componentManager->getComponentCount();
  }

  u32 Coordinator::getSystemsCount() const noexcept {
    return m_systemManager->getSystemsCount();
  }

  bool Coordinator::attachTo( Entity parent, Entity child ) noexcept {

    ASSERT_M( m_entityManager->hasComponent( parent,
                                             m_componentManager->getComponentType<Relationship>() ),
              "Head entity does not contain <Relationship> component in order to create a "
              "hierarchical structure" );
    ASSERT_M( m_entityManager->hasComponent( child,
                                             m_componentManager->getComponentType<Relationship>() ),
              "Child entity does not contain <Relationship> component in order to create a "
              "hierarchical structure" );

    if ( child == parent )
      return false;

    auto &pRelationshipComp = this->getComponent<Relationship>( parent );
    auto &cRelationshipComp = this->getComponent<Relationship>( child );

    if ( cRelationshipComp.parent == parent )
      return false;

    if ( cRelationshipComp.parent != ecs::null )
      detach( child );

    cRelationshipComp.parent = parent;
    auto next_index = pRelationshipComp.firstChild;
    if ( next_index != ecs::null ) {
      this->getComponent<Relationship>( next_index ).prev = child;
    }

    cRelationshipComp.next = next_index;
    cRelationshipComp.prev = ecs::null;
    pRelationshipComp.firstChild = child;
    pRelationshipComp.size++;

    return true;
  }

  void Coordinator::detach( Entity entity ) noexcept {

    ASSERT_M( m_entityManager->hasComponent( entity,
                                             m_componentManager->getComponentType<Relationship>() ),
              "Entity doesn't have relationship compnent to create a hierarchical structure" );


    auto &pRelationshipComp = this->getComponent<Relationship>( entity );

    auto parent_index = pRelationshipComp.parent;
    auto next_index = pRelationshipComp.next;
    auto prev_index = pRelationshipComp.prev;
    // auto first_child_index = pRelationshipComp.firstChild;


    if ( parent_index == ecs::null ) {
      ASSERT_M( next_index == ecs::null, "Hierarchy fail! next_index" );
      ASSERT_M( prev_index == ecs::null, "Hierarchy fail! prev_index" );
    }

    if ( getComponent<Relationship>( parent_index ).firstChild == entity ) {
      // Check if it's the first child in the chain
      ASSERT_M( prev_index == ecs::null, "Hierarcy fail! prev_index" );
      getComponent<Relationship>( parent_index ).firstChild = next_index;

      if ( next_index != ecs::null )
        getComponent<Relationship>( next_index ).prev = ecs::null;
    } else {
      // else is not the first child, but one who has prev and next
      ASSERT_M( prev_index != ecs::null, "Hierarchy fail! prev_index" );
      if ( next_index != ecs::null )
        getComponent<Relationship>( next_index ).prev = prev_index;
      if ( prev_index != ecs::null )
        getComponent<Relationship>( prev_index ).next = next_index;
    }
  }

  Entity Coordinator::getNext( Entity entity ) noexcept {
    return this->getComponent<Relationship>( entity ).next;
  }

  Entity Coordinator::getParent( Entity entity ) noexcept {
    return this->getComponent<Relationship>( entity ).parent;
  }

  Entity Coordinator::getFirst( Entity entity ) noexcept {
    return this->getComponent<Relationship>( entity ).firstChild;
  }

  usize Coordinator::get_relationship_size( Entity entity ) noexcept {
    return this->getComponent<Relationship>( entity ).size;
  }


}    // namespace nile
