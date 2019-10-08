#include "platformer.hh"
#include <Nile/asset/builder/shaderset_builder.hh>
#include <Nile/asset/subsystem/texture_loader.hh>
#include <Nile/ecs/components/renderable.hh>
#include <Nile/ecs/components/transform.hh>
#include <Nile/ecs/components/sprite.hh>


namespace platformer {

  using namespace nile;

  Platformer::Platformer( const std::shared_ptr<nile::GameHost> &gameHost ) noexcept
      : m_gameHost( gameHost )
      , m_inputManager( gameHost->getInputManager() )
      , m_renderer( gameHost->getRenderer() )
      , m_settings( gameHost->getSettings() )
      , m_assetManager( gameHost->getAssetManager() )
      , m_ecsCoordinator( gameHost->getEcsCoordinator() ) {

    // TODO(stel): fix this
    m_camera = std::make_unique<Camera2D>( m_settings );
    m_camera->setScale( glm::vec2( 2.5f ) );
  }


  void Platformer::initialize() noexcept {

    // ECS stuff -----------------------


    // m_ecsCoordinator->addComponent<Transform>(
    //     entity,
    //     Transform {.position = glm::vec3( 1.0f ), .scale = glm::vec3( 1.0f ), .rotation = 1.0f}
    //     );
    // m_ecsCoordinator->addComponent<Renderable>( entity, Renderable {glm::vec3( 1.0f )} );
    //
    // ECS Stuff ends here ------------


    // Register loaders
    m_assetManager->registerLoader<Texture2D, TextureLoader>( true );

    // auto spriteShader = m_assetManager->createBuilder<ShaderSet>()
    //                         .setVertexPath( "../assets/shaders/sprite_vertex.glsl" )
    //                         .setFragmentPath( "../assets/shaders/sprite_fragment.glsl" )
    //                         .build();
    //
    auto fontShader = m_assetManager->createBuilder<ShaderSet>()
                          .setVertexPath( "../assets/shaders/font_vertex.glsl" )
                          .setFragmentPath( "../assets/shaders/font_fragment.glsl" )
                          .build();

    //   m_assetManager->storeAsset<ShaderSet>( "sprite_shader", spriteShader );
    auto spriteShader = m_assetManager->getAsset<ShaderSet>( "sprite_shader" );
    m_assetManager->storeAsset<ShaderSet>( "font_shader", fontShader );

    m_fontRenderer =
        std::make_unique<FontRenderer>( fontShader, m_settings, "../assets/fonts/arial.ttf", 22 );
    spriteShader->use().SetMatrix4( "projection", m_camera->getCameraMatrix() );
    spriteShader->use().SetInteger( "image", 0 );

    m_spriteRenderer = std::make_unique<SpriteRenderer>( spriteShader );


    // Initialize the main character
    m_mainCharacter =
        std::make_unique<MainCharacter>( m_settings, m_inputManager, m_camera, m_assetManager );
    m_mainCharacter->initialize();

    m_enviromentItems =
        std::make_unique<EnviromentItems>( m_settings, m_assetManager, m_spriteRenderer, m_camera );
    m_enviromentItems->initialize();

    m_assetManager->loadAsset<Texture2D>( "background",
                                          "../assets/textures/layers/parallax-demon-woods-bg.png" );

    m_assetManager->loadAsset<Texture2D>(
        "far-trees", "../assets/textures/layers/parallax-demon-woods-far-trees.png" );

    m_assetManager->loadAsset<Texture2D>(
        "mid-trees", "../assets/textures/layers/parallax-demon-woods-mid-trees.png" );

    m_assetManager->loadAsset<Texture2D>(
        "trees", "../assets/textures/layers/parallax-demon-woods-close-trees.png" );

    this->initializeEcs();
  }

  void Platformer::initializeEcs() noexcept {

    const constexpr auto tileWidth = 1078;
    const constexpr auto tileHeight = 224;
    const constexpr auto on_screen_height = 20;


    auto entity = m_ecsCoordinator->createEntity();

    m_ecsCoordinator->addComponent<Transform>(
        entity, Transform {.position = glm::vec3( 0, on_screen_height, 0.0f ),
                           .scale = glm::vec3( tileWidth, tileHeight, 1.0f ),
                           .rotation = 0.0f} );

    m_ecsCoordinator->addComponent<Renderable>( entity, Renderable {.color = glm::vec3( 1.0f )} );

    m_ecsCoordinator->addComponent<SpriteComponent>(entity, SpriteComponent{.texture = m_assetManager->getAsset<Texture2D>("background")});
  }

  void Platformer::draw( f32 deltaTime ) noexcept {

    m_fontRenderer->renderText( "this is working text", 5.0f, 5.0f, 1.0f,
                                glm::vec3( 0.0f, 0.902, 0.463 ) );


    // TODO(stel): hardcoded for now, in the near feature this will be fixed!
    const constexpr auto tileWidth = 1078;
    const constexpr auto tileHeight = 224;
    const constexpr auto runs = 4;
    const constexpr auto on_screen_height = 20;


    for ( int i = 0; i < runs; i++ ) {
      m_spriteRenderer->draw( m_assetManager->getAsset<Texture2D>( "background" ),
                              glm::vec2( i * tileWidth, on_screen_height ),
                              glm::vec2( tileWidth, tileHeight ), 0.0f,
                              glm::vec3( 1.0f, 1.0f, 1.0f ) );
    }

    for ( int i = 0; i < runs; i++ ) {
      // Layer 3 ( layer 3 moves faster than layer 2 )
      m_spriteRenderer->draw(
          m_assetManager->getAsset<Texture2D>( "far-trees" ),
          glm::vec2( ( i * tileWidth ) + m_camera->getPosition().x * 0.3f, on_screen_height ),
          glm::vec2( tileWidth, tileHeight ), 0.0f, glm::vec3( 1.0f, 1.0f, 1.0f ) );
    }

    for ( int i = 0; i < runs; i++ ) {
      // Layer 2 ( layer 2 moves slower than layer 3 and faster than layer 1 )
      m_spriteRenderer->draw(
          m_assetManager->getAsset<Texture2D>( "mid-trees" ),
          glm::vec2( ( i * tileWidth ) + m_camera->getPosition().x * 0.2f, on_screen_height ),
          glm::vec2( tileWidth, tileHeight ), 0.0f, glm::vec3( 1.0f, 1.0f, 1.0f ) );
    }

    for ( int i = 0; i < runs; i++ ) {
      // layer 1 ( the main layer )
      m_spriteRenderer->draw( m_assetManager->getAsset<Texture2D>( "trees" ),
                              glm::vec2( ( i * tileWidth ), on_screen_height ),
                              glm::vec2( tileWidth, tileHeight ), 0.0f,
                              glm::vec3( 1.0f, 1.0f, 1.0f ) );
    }

    // Draw the main character
    m_mainCharacter->draw( deltaTime );
    m_enviromentItems->draw( deltaTime );
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
    m_assetManager->getAsset<ShaderSet>( "sprite_shader" )
        ->use()
        .SetMatrix4( "projection", m_camera->getCameraMatrix() );

    // Update the main character
    m_mainCharacter->update( deltaTime );
  }

}    // namespace platformer
