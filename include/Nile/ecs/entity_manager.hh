/* ================================================================================
$File: entity_manager.hh
$Date: 09 Oct 2019 $
$Revision: $
$Creator: Rostislav Orestis Stelmach
$Notice: $
================================================================================ */

#pragma once

#include "Nile/core/types.hh"

#include <array>
#include <queue>

namespace nile {

  class EntityManager {
  private:
    // Queue of unused entity IDs
    std::queue<Entity> m_availableEntities;

    // Array of signatures where the index corresponds to the entity ID
    std::array<Signature, ecs::MAX_ENTITIES> m_signatures;

    // Totla living entities - used to keep limits on how many exist
    usize m_livingEntities = 0;

  public:
    EntityManager() noexcept;

    [[nodiscard]] Entity createEntity() noexcept;

    void freeEntity(Entity entity) noexcept;

    // Keep track of which components an entity has
    void setSignature( Entity entity, Signature signature ) noexcept;

    // Get the signature ( which components the entity has )
    [[nodiscard]] Signature getSignature( Entity entity ) const noexcept;
  };

}    // namespace nile
