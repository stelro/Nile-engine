/* ================================================================================
$File: font_rendering_system.hh
$Date: 28 Nov 2019 $
$Revision: $
$Creator: Rostislav Orestis Stelmach
$Notice: $
================================================================================ */

#pragma once

#include "Nile/ecs/ecs_system.hh"
#include <memory>

namespace nile {

  class ShaderSet;
  class Settings;
  class Coordinator;

  class FontRenderingSystem : public System {
  private:
    void init_rendering_data() noexcept;
    std::shared_ptr<Coordinator> ecs_coordinator_;
    std::shared_ptr<Settings> settings_;
    std::shared_ptr<ShaderSet> font_shader_;

  public:
    FontRenderingSystem( const std::shared_ptr<Coordinator> &coordinator,
                         const std::shared_ptr<Settings> &settings,
                         const std::shared_ptr<ShaderSet> &shader ) noexcept;
    void create() noexcept;
    void destroy() noexcept;
    void update( float dt ) noexcept;
    void render( float dt ) noexcept;
  };

}    // namespace nile
