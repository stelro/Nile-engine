#pragma once

#include <Nile/asset/asset_manager.hh>
#include <Nile/core/settings.hh>
#include <Nile/core/types.hh>
#include <Nile/ecs/ecs_coordinator.hh>
#include <Nile/renderer/font.hh>

#include <glm/glm.hpp>

#include <memory>

namespace platformer {

  enum class TextPosition {
    CENTER,
    CENTER_UP,
    CENTER_BOTTOM,
    RIGHT_UP,
    LEFT_UP,
    RIGHT_BOTTOM,
    LEFT_BOTTOM,
    LEFT_CENTER,
    RIGHT_CENTER,
  };

  class ScreenText {
  private:
    std::shared_ptr<nile::Settings> m_settings;
    std::shared_ptr<nile::Coordinator> m_ecsCoordinator;
    std::shared_ptr<nile::AssetManager> m_assetManager;

    nile::Entity m_entity;

  public:
    ScreenText( std::shared_ptr<nile::Settings> &settings,
                std::shared_ptr<nile::Coordinator> &coordinator,
                const std::shared_ptr<nile::AssetManager> &assetManager ) noexcept;

    ~ScreenText() noexcept ;

    void print( const std::string &text, const glm::vec2 &position = glm::vec2( 20.0f, 20.0f ),
                const glm::vec3 col = glm::vec3( 0.9f, 0.2f, 0.2f ) ) noexcept;
  };

}    // namespace platformer
