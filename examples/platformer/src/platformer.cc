#include "platformer.hh"
#include <Nile/asset/builder/shaderset_builder.hh>
#include <Nile/asset/subsystem/texture_loader.hh>
#include <Nile/ecs/components/camera_component.hh>
#include <Nile/ecs/components/mesh_component.hh>
#include <Nile/ecs/components/primitive.hh>
#include <Nile/ecs/components/renderable.hh>
#include <Nile/ecs/components/sprite.hh>
#include <Nile/ecs/components/transform.hh>


namespace platformer {

  using namespace nile;

  Platformer::Platformer( const std::shared_ptr<nile::GameHost> &gameHost ) noexcept
      : m_gameHost( gameHost )
      , m_inputManager( gameHost->getInputManager() )
      , m_renderer( gameHost->getRenderer() )
      , m_settings( gameHost->getSettings() )
      , m_assetManager( gameHost->getAssetManager() )
      , m_ecsCoordinator( gameHost->getEcsCoordinator() ) {
  

        m_lastX = m_settings->getWidth() / 2.0f;
        m_lastY = m_settings->getHeight() / 2.0f;

      }


  void Platformer::initialize() noexcept {


    // Register loaders
    m_assetManager->registerLoader<Texture2D, TextureLoader>( true );

    auto fontShader = m_assetManager->createBuilder<ShaderSet>()
                          .setVertexPath( "../assets/shaders/font_vertex.glsl" )
                          .setFragmentPath( "../assets/shaders/font_fragment.glsl" )
                          .build();

    auto spriteShader = m_assetManager->getAsset<ShaderSet>( "sprite_shader" );
    m_assetManager->storeAsset<ShaderSet>( "font_shader", fontShader );

    m_fontRenderer =
        std::make_unique<FontRenderer>( fontShader, m_settings, "../assets/fonts/arial.ttf", 22 );

    m_spriteRenderer = std::make_unique<SpriteRenderer>( spriteShader );


    m_assetManager->loadAsset<Texture2D>( "background",
                                          "../assets/textures/layers/parallax-demon-woods-bg.png" );

    m_assetManager->loadAsset<Texture2D>(
        "far-trees", "../assets/textures/layers/parallax-demon-woods-far-trees.png" );

    m_assetManager->loadAsset<Texture2D>(
        "mid-trees", "../assets/textures/layers/parallax-demon-woods-mid-trees.png" );

    m_assetManager->loadAsset<Texture2D>(
        "trees", "../assets/textures/layers/parallax-demon-woods-close-trees.png" );

    m_assetManager->loadAsset<Texture2D>( "grid", "../assets/textures/grid.jpg" );

    m_cameraEntity = m_ecsCoordinator->createEntity();
    Transform camera_transform;
    camera_transform.position = glm::vec3( 0.0f, 0.0f, 3.0f );
    m_ecsCoordinator->addComponent<Transform>( m_cameraEntity, camera_transform );

    CameraComponent cameraComponent( 0.1f, 100.0f, 45.0f, ProjectionType::PERSPECTIVE );
    m_ecsCoordinator->addComponent<CameraComponent>( m_cameraEntity, cameraComponent );


    //  this->initializeEcs();
    //  this->createAxisLines();
    this->test3d();
  }

  void Platformer::initializeEcs() noexcept {

    const constexpr auto tileWidth = 1078;
    const constexpr auto tileHeight = 224;
    const constexpr auto on_screen_height = 20;

    Entity entities[ 4 ];
    for ( auto i = 0; i < 4; i++ ) {
      entities[ i ] = m_ecsCoordinator->createEntity();
    }

    m_cameraEntity = m_ecsCoordinator->createEntity();
    m_ecsCoordinator->addComponent<Transform>( m_cameraEntity, Transform() );

    CameraComponent cameraComponent( 0.1f, 100.0f, 45.0f, ProjectionType::PERSPECTIVE );
    m_ecsCoordinator->addComponent<CameraComponent>( m_cameraEntity, cameraComponent );

    Transform transform( glm::vec3( 0, 1.0f, 0.0f ), glm::vec3( 1.0f, 1.0f, 1.0f ), 0.0f );


    Renderable renderable( glm::vec3( 1.0f ) );


    // Background
    m_ecsCoordinator->addComponent<Transform>( entities[ 0 ], transform );
    m_ecsCoordinator->addComponent<Renderable>( entities[ 0 ], renderable );
    m_ecsCoordinator->addComponent<SpriteComponent>(
        entities[ 0 ], SpriteComponent( m_assetManager->getAsset<Texture2D>( "background" ) ) );

    // Far trees
    m_ecsCoordinator->addComponent<Transform>( entities[ 1 ], transform );
    m_ecsCoordinator->addComponent<Renderable>( entities[ 1 ], renderable );
    m_ecsCoordinator->addComponent<SpriteComponent>(
        entities[ 1 ], SpriteComponent( m_assetManager->getAsset<Texture2D>( "far-trees" ) ) );

    // mid trees
    m_ecsCoordinator->addComponent<Transform>( entities[ 2 ], transform );
    m_ecsCoordinator->addComponent<Renderable>( entities[ 2 ], renderable );
    m_ecsCoordinator->addComponent<SpriteComponent>(
        entities[ 2 ], SpriteComponent( m_assetManager->getAsset<Texture2D>( "mid-trees" ) ) );

    // trees
    m_ecsCoordinator->addComponent<Transform>( entities[ 3 ], transform );
    m_ecsCoordinator->addComponent<Renderable>( entities[ 3 ], renderable );
    m_ecsCoordinator->addComponent<SpriteComponent>(
        entities[ 3 ], SpriteComponent( m_assetManager->getAsset<Texture2D>( "trees" ) ) );
  }

  void Platformer::createAxisLines() noexcept {
    // primitive test
    Primitive primitive;
    primitive.begin = glm::vec2( 0.0f, 0.0f );
    primitive.end = glm::vec2( 100.0f, 0.0f );
    primitive.lineWidth = 2.2f;

    Transform primitive_transform( glm::vec3( 22.0f, m_settings->getHeight() - 18.0f, 0.0f ),
                                   glm::vec3( 1.0f ), 0.0f );

    auto x_axis_line = m_ecsCoordinator->createEntity();
    m_ecsCoordinator->addComponent<Transform>( x_axis_line, primitive_transform );
    m_ecsCoordinator->addComponent<Renderable>( x_axis_line,
                                                Renderable( glm::vec3( 0.957f, 0.263f, 0.212f ) ) );
    m_ecsCoordinator->addComponent<Primitive>( x_axis_line, primitive );

    primitive_transform.rotation = 270.0f;

    auto y_axis_line = m_ecsCoordinator->createEntity();
    m_ecsCoordinator->addComponent<Transform>( y_axis_line, primitive_transform );
    m_ecsCoordinator->addComponent<Renderable>( y_axis_line,
                                                Renderable( glm::vec3( 0.298f, 0.686f, 0.314f ) ) );
    m_ecsCoordinator->addComponent<Primitive>( y_axis_line, primitive );

    primitive_transform.rotation = 315.0f;

    primitive.end.x = 80.0f;
    auto z_axis_line = m_ecsCoordinator->createEntity();
    m_ecsCoordinator->addComponent<Transform>( z_axis_line, primitive_transform );
    m_ecsCoordinator->addComponent<Renderable>( z_axis_line,
                                                Renderable( glm::vec3( 0.157f, 0.208f, 0.576f ) ) );
    m_ecsCoordinator->addComponent<Primitive>( z_axis_line, primitive );
  }

  void Platformer::draw( f32 deltaTime ) noexcept {

    m_fontRenderer->renderText( "this is working text", 5.0f, 5.0f, 1.0f,
                                glm::vec3( 0.0f, 0.902, 0.463 ) );
  }


  void Platformer::update( f32 deltaTime ) noexcept {

    const f32 zoomScale = 0.02f;
    const f32 cameraSpeed = 0.02f * deltaTime;

    auto &camera_transform = m_ecsCoordinator->getComponent<Transform>( m_cameraEntity );
    auto &camera_component = m_ecsCoordinator->getComponent<CameraComponent>( m_cameraEntity );

    if ( m_inputManager->isKeyPressed( SDLK_ESCAPE ) ) {
      m_inputManager->terminateEngine();
    }

    if ( m_inputManager->isKeyPressed( SDLK_r ) ) {
      camera_transform.scale += glm::vec3( zoomScale );
      camera_component.shouldCameraUpdate = true;
    }

    if ( m_inputManager->isKeyPressed( SDLK_e ) ) {
      camera_transform.scale -= glm::vec3( zoomScale );
      camera_component.shouldCameraUpdate = true;
    }

    if ( m_inputManager->isKeyPressed( SDLK_w ) ) {
      camera_transform.position += cameraSpeed * camera_component.cameraFront;
      camera_component.shouldCameraUpdate = true;
    }

    if ( m_inputManager->isKeyPressed( SDLK_s ) ) {
      camera_transform.position -= cameraSpeed * camera_component.cameraFront;
      camera_component.shouldCameraUpdate = true;
    }

    if ( m_inputManager->isKeyPressed( SDLK_a ) ) {
      camera_transform.position -=
          glm::normalize( glm::cross( camera_component.cameraFront, camera_component.cameraUp ) ) *
          cameraSpeed;
      camera_component.shouldCameraUpdate = true;
    }

    if ( m_inputManager->isKeyPressed( SDLK_d ) ) {
      camera_transform.position +=
          glm::normalize( glm::cross( camera_component.cameraFront, camera_component.cameraUp ) ) *
          cameraSpeed;
      camera_component.shouldCameraUpdate = true;
    }


    if ( m_mouse_pos != m_inputManager->getMousePos() && m_inputManager->mouseLeftPressed() ) {


      if ( m_firstMouse ) {
        m_lastX = m_mouse_pos.x;
        m_lastY = m_mouse_pos.y;
        m_firstMouse = false;
      }

      f32 xoffset = m_mouse_pos.x - m_lastX;
      f32 yoffset = m_lastY - m_mouse_pos.y;

      m_lastX = m_mouse_pos.x;
      m_lastY = m_mouse_pos.y;

      f32 sensitivity = 0.05f;
      xoffset *= sensitivity;
      yoffset *= sensitivity;

      yaw += xoffset;
      pitch += yoffset;

      if ( pitch > 89.0f )
        pitch = 89.0f;
      if ( pitch < -89.0f )
        pitch = -89.0f;

      glm::vec3 front;
      front.x = cos( glm::radians( yaw ) ) * cos( glm::radians( pitch ) );
      front.y = sin( glm::radians( pitch ) );
      front.z = sin( glm::radians( yaw ) ) * cos( glm::radians( pitch ) );

      camera_component.cameraFront = glm::normalize( front );
      camera_component.shouldCameraUpdate = true;
    }


    // We set projection matrix to the object that are "moving"
    // since the camera is static, and we shift the world
    m_assetManager->getAsset<ShaderSet>( "sprite_shader" )
        ->use()
        .SetMatrix4( "projection", camera_component.cameraMatrix );

    m_assetManager->getAsset<ShaderSet>( "sprite_shader" )
        ->use()
        .SetMatrix4( "view", camera_component.viewMatrix );

    m_assetManager->getAsset<ShaderSet>( "line_shader" )
        ->use()
        .SetMatrix4( "projection", camera_component.cameraMatrix );

    m_assetManager->getAsset<ShaderSet>( "mesh_shader" )
        ->use()
        .SetMatrix4( "view", camera_component.viewMatrix );

    m_assetManager->getAsset<ShaderSet>( "mesh_shader" )
        ->use()
        .SetMatrix4( "projection", camera_component.cameraMatrix );

    m_mouse_pos = m_inputManager->getMousePos();
  }


  void Platformer::test3d() noexcept {

    auto entity = m_ecsCoordinator->createEntity();

    Transform transform;
    transform.position = glm::vec3( 0.0f, 0.0f, -3.0f );
    m_ecsCoordinator->addComponent<Transform>( entity, transform );

    Renderable renderable;
    renderable.color = glm::vec3( 1.0f, 1.0f, 1.0f );
    m_ecsCoordinator->addComponent<Renderable>( entity, renderable );

    MeshComponent mesh;
    mesh.texture = m_assetManager->getAsset<Texture2D>( "grid" );
    m_ecsCoordinator->addComponent<MeshComponent>( entity, mesh );
  }

}    // namespace platformer
