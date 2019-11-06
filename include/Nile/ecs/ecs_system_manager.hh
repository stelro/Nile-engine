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
#include "Nile/ecs/ecs_field_detection.hh"
#include "Nile/ecs/ecs_system.hh"

#include <functional>
#include <memory>
#include <unordered_map>
#include <vector>

namespace nile {

  class EcsSystemManager {
  private:
    using update_handle = std::function<void( f32 )>;
    using render_handle = std::function<void( f32 )>;
    using create_handle = std::function<void()>;
    using destroy_handle = std::function<void()>;

    // Map from system type string pointer to a signature
    std::unordered_map<const char *, Signature> m_signatures;

    // Map from system type string pointer to system pointer
    std::unordered_map<const char *, std::shared_ptr<System>> m_systems;

    // Update handlers for every system ( we try to avoid virtual inheritance )
    std::vector<update_handle> m_updateHandles;

    // Render handlers for every system
    std::vector<render_handle> m_renderHandles;

    // Create handler ( those are the create() method handlers from the ecs
    // system manager ) and they're called right before the first update method
    // and not on the moment of the registtrtion of the system to ecs_system_manager,
    // so we assume, that there is some components available
    std::vector<create_handle> m_createHandles;

    template <typename T>
    void registerUpdateHandle( const T &t ) {
      m_updateHandles.push_back( [=]( float dt ) { t->update( dt ); } );
    }

    template <typename T>
    void registerRenderHandle( const T &t ) {
      m_updateHandles.push_back( [=]( float dt ) { t->render( dt ); } );
    }

    template <typename T>
    void registerCreateHandle( const T &t ) {
      m_createHandles.push_back( [=]() { t->create(); } );
    }

  public:
    template <typename T, typename... Args>
    std::shared_ptr<T> registerSystem( Args &&... args ) noexcept {
      const char *typeName = typeid( T ).name();

      ASSERT_M( m_systems.find( typeName ) == m_systems.end(),
                " Registering system more than once" );

      // Create a pointer to the system and return it so it can be used externally
      auto system = std::make_shared<T>( std::forward<Args>( args )... );

      // If the system has update function, register it and save it for later use
      // so we can call it from this class update method
      if constexpr ( ecs_has_create<T> )
        this->registerCreateHandle( system );
      if constexpr ( ecs_has_update<T> )
        this->registerUpdateHandle( system );
      if constexpr ( ecs_has_render<T> )
        this->registerRenderHandle( system );

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

    // Calls all create handles, this should be called right before update method
    void createSystems() noexcept;

    void update( float dt ) noexcept;

    void render( float dt ) noexcept;
    
    u32 getSystemsCount() const noexcept;
  };

}    // namespace nile
