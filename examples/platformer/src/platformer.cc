#include "platformer.hh"
//#include "npc/main_character.hh"

namespace platformer {

  using namespace nile;

  Platformer::Platformer( const std::shared_ptr<nile::GameHost> &gameHost ) noexcept
      : m_gameHost( gameHost )
      , m_inputManager( gameHost->getInputManager() )
      , m_renderer( gameHost->getRenderer() )
      , m_settings( gameHost->getSettings() ) {
    // TODO(stel): fix this
    m_camera = std::make_unique<Camera2D>( m_settings );
    m_camera->setScale( glm::vec2( 2.5f ) );
  }


  void Platformer::initialize() noexcept {

    // Initialize the main character
    m_mainCharacter = std::make_unique<MainCharacter>( m_settings, m_inputManager, m_camera );
    m_mainCharacter->initialize();

    ResourceManager::loadShader( "../assets/shaders/sprite_vertex.glsl",
                                 "../assets/shaders/sprite_fragment.glsl", {}, "sprite" );

    auto font_shader =
        ResourceManager::loadShader( "../assets/shaders/font_vertex.glsl",
                                     "../assets/shaders/font_fragment.glsl", {}, "font_shader" );

    m_fontRenderer =
        std::make_unique<FontRenderer>( font_shader, m_settings, "../assets/fonts/arial.ttf", 22 );

    auto sprite_shader = ResourceManager::getShader( "sprite" );
    sprite_shader->use().SetMatrix4( "projection", m_camera->getCameraMatrix() );
    sprite_shader->use().SetInteger( "image", 0 );

    m_spriteRenderer = std::make_unique<SpriteRenderer>( sprite_shader );

    ResourceManager::loadTexture( "../assets/textures/layers/parallax-demon-woods-bg.png", true,
                                  "background" );

    ResourceManager::loadTexture( "../assets/textures/layers/parallax-demon-woods-far-trees.png",
                                  true, "far-trees" );

    ResourceManager::loadTexture( "../assets/textures/layers/parallax-demon-woods-mid-trees.png",
                                  true, "mid-trees" );

    ResourceManager::loadTexture( "../assets/textures/layers/parallax-demon-woods-close-trees.png",
                                  true, "trees" );

    ResourceManager::loadTexture( "../assets/textures/skeleton/skeleton-attack.png", true,
                                  "skeleton-attack" );
  }

  void Platformer::draw( f32 deltaTime ) noexcept {

    m_fontRenderer->renderText( "this is working text", 5.0f, 5.0f, 1.0f, glm::vec3( 0.0f,  0.902, 0.463) );


    // log::print( m_shouldHaltTheEvents ? "true\n" : "false\n" );

    // TODO(stel): hardcoded for now, in the near feature this will be fixed!
    const constexpr auto tileWidth = 1078;
    const constexpr auto tileHeight = 224;
    const constexpr auto runs = 4;
    const constexpr auto on_screen_height = 20;

    for ( int i = 0; i < runs; i++ ) {
      // The background layer
      m_spriteRenderer->draw(
          ResourceManager::getTexture( "background" ), glm::vec2( i * tileWidth, on_screen_height ),
          glm::vec2( tileWidth, tileHeight ), 0.0f, glm::vec3( 1.0f, 1.0f, 1.0f ) );
    }

    for ( int i = 0; i < runs; i++ ) {
      // Layer 3 ( layer 3 moves faster than layer 2 )
      m_spriteRenderer->draw(
          ResourceManager::getTexture( "far-trees" ),
          glm::vec2( ( i * tileWidth ) + m_camera->getPosition().x * 0.3f, on_screen_height ),
          glm::vec2( tileWidth, tileHeight ), 0.0f, glm::vec3( 1.0f, 1.0f, 1.0f ) );
    }

    for ( int i = 0; i < runs; i++ ) {
      // Layer 2 ( layer 2 moves slower than layer 3 and faster than layer 1 )
      m_spriteRenderer->draw(
          ResourceManager::getTexture( "mid-trees" ),
          glm::vec2( ( i * tileWidth ) + m_camera->getPosition().x * 0.2f, on_screen_height ),
          glm::vec2( tileWidth, tileHeight ), 0.0f, glm::vec3( 1.0f, 1.0f, 1.0f ) );
    }

    for ( int i = 0; i < runs; i++ ) {
      // layer 1 ( the main layer )
      m_spriteRenderer->draw(
          ResourceManager::getTexture( "trees" ), glm::vec2( ( i * tileWidth ), on_screen_height ),
          glm::vec2( tileWidth, tileHeight ), 0.0f, glm::vec3( 1.0f, 1.0f, 1.0f ) );
    }

    // Draw the main character
    m_mainCharacter->draw( deltaTime );
  }


  void Platformer::update( f32 deltaTime ) noexcept {

    const constexpr f32 zoomScale = 0.1f;

    if ( m_inputManager->isKeyPressed( SDLK_ESCAPE ) ) {
      m_inputManager->terminateEngine();
    }

    if ( m_inputManager->isKeyPressed( SDLK_r ) ) {
      m_camera->setScale( m_camera->getScale() + glm::vec2( zoomScale ) );
    }

    if ( m_inputManager->isKeyPressed( SDLK_e ) ) {
      m_camera->setScale( m_camera->getScale() - glm::vec2( zoomScale ) );
    }


    m_camera->update( deltaTime );

    // We set projection matrix to the object that are "moving"
    // since the camera is static, and we shift the world
    ResourceManager::getShader( "sprite" )
        ->use()
        .SetMatrix4( "projection", m_camera->getCameraMatrix() );

    // Update the main character
    m_mainCharacter->update( deltaTime );
  }

}    // namespace platformer
