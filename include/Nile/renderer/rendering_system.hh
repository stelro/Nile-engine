/* ================================================================================
$File: rendering_system.hh
$Date: 09 Oct 2019 $
$Revision: $
$Creator: Rostislav Orestis Stelmach
$Notice: $
================================================================================ */

#pragma once

#include "Nile/core/types.hh"
#include "Nile/ecs/ecs_system.hh"

#include <memory>
#include <mutex>

namespace nile {

  class Coordinator;
  class VulkanRenderingDevice;
  class ShaderSet;

  class RenderingSystem : public System {
  private:
    std::shared_ptr<Coordinator> m_ecsCoordinator;
    std::shared_ptr<VulkanRenderingDevice> m_vulkanRenderer;
    u32 m_vao;
    void initRenderData() noexcept;

    std::once_flag m_flag1;

    int m_loading_falg = 0;

  public:
    RenderingSystem( const std::shared_ptr<Coordinator> &coordinator, const std::shared_ptr<VulkanRenderingDevice>& vulkanRenderer ) noexcept;
    void create() noexcept;
    void update( float dt ) noexcept;
    void render( float dt ) noexcept;
  };
}    // namespace nile
