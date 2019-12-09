#pragma once

#include "screen_text.hh"

#include <Nile/asset/asset_manager.hh>
#include <Nile/core/settings.hh>
#include <Nile/core/types.hh>
#include <Nile/core/timer.hh>
#include <Nile/core/types.hh>
#include <Nile/ecs/ecs_coordinator.hh>
#include <Nile/renderer/font.hh>

#include <glm/glm.hpp>

#include <memory>
#include <stack>
#include <unordered_map>
#include <utility>


namespace platformer {

  struct BufferElement {
    std::unique_ptr<ScreenText> text;
    nile::Timer timer;
  };

  class TextBuffer {
  private:
    std::shared_ptr<nile::Settings> m_settings;
    std::shared_ptr<nile::Coordinator> m_ecsCoordinator;
    std::shared_ptr<nile::AssetManager> m_assetManager;

    std::stack<BufferElement> m_elementsStack;

  public:
    TextBuffer( std::shared_ptr<nile::Settings> &settings,
                std::shared_ptr<nile::Coordinator> &coordinator,
                const std::shared_ptr<nile::AssetManager> &assetManager ) noexcept;

    void append( const std::string &text, const glm::vec2 &position = glm::vec2( 20.0f, 20.0f ),
                 const glm::vec3 col = glm::vec3( 0.9f, 0.2f, 0.2f ) ) noexcept;

    void update(nile::f32 deltaTime) noexcept;
  };

}    // namespace platformer
