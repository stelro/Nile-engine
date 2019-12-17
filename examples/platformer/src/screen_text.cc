#include "screen_text.hh"

#include "Nile/asset/asset_manager.hh"
#include "Nile/core/file_system.hh"
#include "Nile/core/settings.hh"
#include "Nile/core/timer.hh"
#include "Nile/ecs/components/font_component.hh"
#include "Nile/ecs/components/renderable.hh"
#include "Nile/ecs/components/transform.hh"
#include "Nile/ecs/ecs_coordinator.hh"
#include "Nile/renderer/shaderset.hh"


namespace platformer {

  using namespace nile;

  ScreenText::ScreenText( std::shared_ptr<Settings> &settings,
                          std::shared_ptr<Coordinator> &coordinator,
                          const std::shared_ptr<AssetManager> &assetManager ) noexcept
      : m_settings( settings )
      , m_ecsCoordinator( coordinator )
      , m_assetManager( assetManager ) {

    auto fontAsset = m_assetManager->loadAsset<Font>(
        "screen_text_font", FileSystem::getPath( "assets/fonts/LiberationMono-Regular.ttf" ) );

    m_entity = m_ecsCoordinator->createEntity();

    FontComponent font;
    font.font = fontAsset;
    Renderable renderable;

    Transform transform;
    transform.position = glm::vec3( 20.0f, 20.0f, 0.0f );
    transform.scale = glm::vec3( 1.0f );

    font.text = "";
    renderable.color = glm::vec3( 0.9f, 0.2f, 0.2f );

    m_ecsCoordinator->addComponent<Transform>( m_entity, transform );
    m_ecsCoordinator->addComponent<Renderable>( m_entity, renderable );
    m_ecsCoordinator->addComponent<FontComponent>( m_entity, font );
  }

  ScreenText::~ScreenText() noexcept {}

  void ScreenText::print( const std::string &text, const glm::vec2 &position,
                          const glm::vec3 col ) noexcept {

    auto &font = m_ecsCoordinator->getComponent<FontComponent>( m_entity );
    auto &transform = m_ecsCoordinator->getComponent<Transform>( m_entity );
    auto &renderable = m_ecsCoordinator->getComponent<Renderable>( m_entity );

    font.text = text;
    transform.position = glm::vec3( position, 0.0f );
    renderable.color = col;


  }    // namespace platformer


  glm::vec2 textPosition( TextPosition textPos ) noexcept {
    // const auto top_offset = 42.f;
    // const auto bottom_offset = 42.f;
    // const auto right_offset = 42.f;
    // const auto left_offset = 42.f;
    //
    // switch ( textPos ) {
    //   case TextPosition::CENTER:
    //     transform.position.x =
    //         static_cast<f32>( m_settings->getWidth() ) / 2.0f - ( font.width / 2 );
    //     transform.position.y = static_cast<f32>( m_settings->getHeight() ) / 2.0f;
    //     break;
    //   case TextPosition::CENTER_UP:
    //     transform.position.x =
    //         static_cast<f32>( m_settings->getWidth() ) / 2.0f - ( font.width / 2 );
    //     transform.position.y = top_offset;
    //     break;
    //   case TextPosition::CENTER_BOTTOM:
    //     transform.position.x =
    //         static_cast<f32>( m_settings->getWidth() ) / 2.0f - ( font.width / 2 );
    //     transform.position.y = ( static_cast<f32>( m_settings->getHeight() ) ) - bottom_offset;
    //     break;
    //   case TextPosition::RIGHT_UP:
    //     transform.position.x =
    //         static_cast<f32>( m_settings->getWidth() ) - right_offset - ( font.width );
    //     transform.position.y = top_offset;
    //     break;
    //   case TextPosition::LEFT_UP:
    //     transform.position.x = left_offset;
    //     transform.position.y = top_offset;
    //     break;
    //   case TextPosition::RIGHT_BOTTOM:
    //     transform.position.x =
    //         static_cast<f32>( m_settings->getWidth() ) - right_offset - ( font.width );
    //     transform.position.y = ( static_cast<f32>( m_settings->getHeight() ) ) - bottom_offset;
    //     break;
    //   case TextPosition::LEFT_BOTTOM:
    //     transform.position.x = left_offset;
    //     transform.position.y = static_cast<f32>( m_settings->getHeight() ) - bottom_offset;
    //     break;
    //   case TextPosition::LEFT_CENTER:
    //     transform.position.x = left_offset;
    //     transform.position.y = static_cast<f32>( m_settings->getHeight() ) / 2;
    //     break;
    //   case TextPosition::RIGHT_CENTER:
    //     transform.position.x =
    //         static_cast<f32>( m_settings->getWidth() ) - right_offset - ( font.width );
    //     transform.position.y = static_cast<f32>( m_settings->getHeight() ) / 2;
    //     break;
    // }
    //
    return glm::vec2 {};
  }

}    // namespace platformer
