#include "Nile/renderer/rendering_system.hh"
#include "Nile/ecs/components/mesh_component.hh"
#include "Nile/ecs/components/renderable.hh"
#include "Nile/ecs/components/transform.hh"
#include "Nile/ecs/ecs_coordinator.hh"
#include "Nile/log/log.hh"
#include "Nile/renderer/shaderset.hh"
#include "Nile/renderer/texture2d.hh"
#include "Nile/utils/vertex.hh"

#include "Nile/drivers/vulkan/vulkan_buffer.hh"
#include "Nile/drivers/vulkan/vulkan_rendering_device.hh"
#include "Nile/drivers/vulkan/vulkan_vertex.hh"

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace nile {

  [[maybe_unused]] static void loadingLoop( int i ) {
    if ( i >= 0 && i <= 999 )
      log::print( "\r | " );
    else if ( i >= 1000 && i <= 1999 )
      log::print( "\r / " );
    else if ( i >= 2000 && i <= 2999 )
      log::print( "\r - " );
    else if ( i >= 3000 && i <= 3999 )
      log::print( "\r \\ " );
    else if ( i >= 4000 && i <= 4999 )
      log::print( "\r / " );
    else
      log::print( "\r - " );
  }

  RenderingSystem::RenderingSystem(
      const std::shared_ptr<Coordinator> &coordinator,
      const std::shared_ptr<VulkanRenderingDevice> &vulkanRenderer ) noexcept
      : m_ecsCoordinator( coordinator )
      , m_vulkanRenderer( vulkanRenderer ) {}


  void RenderingSystem::create() noexcept {
    this->initRenderData();
    log::notice( "RenderingSystem::create()\n" );
  }

  void RenderingSystem::update( float dt ) noexcept {}

  void RenderingSystem::render( float dt ) noexcept {

    std::call_once( m_flag1, []() { log::notice( "RenderingSystem::render()\n" ); } );

    m_vulkanRenderer->draw();

    for ( const auto &entity : m_entities ) {

      // Perform here all the meshes transformations and rendering related stuff

      [[maybe_unused]] auto &mesh = m_ecsCoordinator->getComponent<MeshComponent>( entity );
      [[maybe_unused]] auto &transform = m_ecsCoordinator->getComponent<Transform>( entity );
      [[maybe_unused]] auto &renderable = m_ecsCoordinator->getComponent<Renderable>( entity );
    }
  }

  void RenderingSystem::initRenderData() noexcept {

    std::vector<VulkanVertex> verticies;
    std::vector<u32> indices;

    log::warning("RenderingSystem::initRenderData()\n");

    for ( const auto &entity : m_entities ) {

      [[maybe_unused]] auto &mesh = m_ecsCoordinator->getComponent<MeshComponent>( entity );

//      m_vulkanRenderer->setVertexBuffer(mesh.vertices);
 //     m_vulkanRenderer->setIndexBuffer(mesh.indices);
    }

  //  m_vulkanRenderer->createCommandBuffers();
  }

}    // namespace nile
