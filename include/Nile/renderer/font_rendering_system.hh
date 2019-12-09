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
    void initRenderData() noexcept;
    std::shared_ptr<Coordinator> m_ecsCoordinator;
    std::shared_ptr<Settings> m_settings;
    std::shared_ptr<ShaderSet> m_fontShader;

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
