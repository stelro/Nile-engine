/* ================================================================================
$File: ecs_system_manager.hh
$Date: 09 Oct 2019 $
$Revision: $
$Creator: Rostislav Orestis Stelmach
$Notice: $
================================================================================ */

#pragma once

#include "Nile/core/assert.hh"
#include "Nile/core/types.hh"
#include "Nile/ecs/ecs_system.hh"

#include <memory>
#include <unordered_map>

namespace nile {

  class EcsSystemManager {
  private:
    // Map from system type string pointer to a signature
    std::unordered_map<const char *, Signature> m_signatures;

    // Map from system type string pointer to system pointer
    std::unordered_map<const char *, std::shared_ptr<System>> m_systems;

  public:
    template <typename T, typename... Args>
    std::shared_ptr<T> registerSystem( Args &&... args ) noexcept {
      const char *typeName = typeid( T ).name();

      ASSERT_M( m_systems.find( typeName ) == m_systems.end(),
                " Registering system more than once" );

      // Create a pointer to the system and return it so it can be used externally
      auto system = std::make_shared<T>( std::forward<Args>( args )... );
      m_systems.insert( {typeName, system} );
      return system;
    }

    template <typename T>
    void setSignature( Signature signature ) noexcept {
      const char *typeName = typeid( T ).name();

      ASSERT_M( m_systems.find( typeName ) != m_systems.end(), "System used before registered." );

      // Set the signature for this system
      m_signatures.insert( {typeName, signature} );
    }

    void entityDestroyed( Entity entity ) noexcept;

    void entitySignatureChanged( Entity entity, Signature entitySignature ) noexcept;
  };

}    // namespace nile
