#include "platformer.hh"
#include <Nile/asset/builder/model_builder.hh>
#include <Nile/asset/builder/shaderset_builder.hh>
#include <Nile/asset/subsystem/texture_loader.hh>
#include <Nile/core/file_system.hh>
#include <Nile/ecs/components/camera_component.hh>
#include <Nile/ecs/components/mesh_component.hh>
#include <Nile/ecs/components/primitive.hh>
#include <Nile/ecs/components/renderable.hh>
#include <Nile/ecs/components/sprite.hh>
#include <Nile/ecs/components/transform.hh>
#include <Nile/math/utils.hh>
#include <Nile/renderer/model.hh>

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

    auto fontShader =
        m_assetManager->createBuilder<ShaderSet>()
            .setVertexPath( FileSystem::getPath( "assets/shaders/font_vertex.glsl" ) )
            .setFragmentPath( FileSystem::getPath( "assets/shaders/font_fragment.glsl" ) )
            .build();

    auto spriteShader = m_assetManager->getAsset<ShaderSet>( "sprite_shader" );
    m_assetManager->storeAsset<ShaderSet>( "font_shader", fontShader );

    auto modelShader =
        m_assetManager->createBuilder<ShaderSet>()
            .setVertexPath( FileSystem::getPath( "assets/shaders/model_vertex.glsl" ) )
            .setFragmentPath( FileSystem::getPath( "assets/shaders/model_fragment.glsl" ) )
            .build();

    m_assetManager->storeAsset<ShaderSet>( "model_shader", modelShader );


    m_fontRenderer = std::make_unique<FontRenderer>(
        fontShader, m_settings, FileSystem::getPath( "assets/fonts/arial.ttf" ), 22 );

    m_spriteRenderer = std::make_unique<SpriteRenderer>( spriteShader );


    m_assetManager->loadAsset<Texture2D>( "background",
                                          "../assets/textures/layers/parallax-demon-woods-bg.png" );

    m_assetManager->loadAsset<Texture2D>(
        "far-trees",
        FileSystem::getPath( "assets/textures/layers/parallax-demon-woods-far-trees.png" ) );

    m_assetManager->loadAsset<Texture2D>(
        "mid-trees",
        FileSystem::getPath( "assets/textures/layers/parallax-demon-woods-mid-trees.png" ) );

    m_assetManager->loadAsset<Texture2D>(
        "trees",
        FileSystem::getPath( "assets/textures/layers/parallax-demon-woods-close-trees.png" ) );

    m_assetManager->loadAsset<Texture2D>( "grid",
                                          FileSystem::getPath( "assets/textures/grid.jpg" ) );
    m_assetManager->loadAsset<Texture2D>( "container",
                                          FileSystem::getPath( "assets/textures/container.jpg" ) );


    m_cameraEntity = m_ecsCoordinator->createEntity();
    Transform camera_transform;
    camera_transform.position = glm::vec3( -12.0f, 20.0f, 70.0f );
    m_ecsCoordinator->addComponent<Transform>( m_cameraEntity, camera_transform );

    CameraComponent cameraComponent( 0.1f, 1000.0f, 45.0f, ProjectionType::PERSPECTIVE );
    m_ecsCoordinator->addComponent<CameraComponent>( m_cameraEntity, cameraComponent );


    //  this->initializeEcs();
    // this->test3d();
    //   this->createAxisLines();
    this->testModel();
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
    primitive.end = glm::vec2( 1.0f, 0.0f );

    Transform primitive_transform( glm::vec3( 0.0f, 1.0f, -3.0f ), glm::vec3( 1.0f ), 0.0f );

    auto x_axis_line = m_ecsCoordinator->createEntity();
    m_ecsCoordinator->addComponent<Transform>( x_axis_line, primitive_transform );
    m_ecsCoordinator->addComponent<Renderable>( x_axis_line,
                                                Renderable( glm::vec3( 0.957f, 0.263f, 0.212f ) ) );
    m_ecsCoordinator->addComponent<Primitive>( x_axis_line, primitive );
    //
    // primitive_transform.rotation = 270.0f;
    //
    // auto y_axis_line = m_ecsCoordinator->createEntity();
    // m_ecsCoordinator->addComponent<Transform>( y_axis_line, primitive_transform );
    // m_ecsCoordinator->addComponent<Renderable>( y_axis_line,
    //                                             Renderable( glm::vec3( 0.298f, 0.686f, 0.314f ) )
    //                                             );
    // m_ecsCoordinator->addComponent<Primitive>( y_axis_line, primitive );
    //
    // primitive_transform.rotation = 315.0f;
    //
    // primitive.end.x = 80.0f;
    // auto z_axis_line = m_ecsCoordinator->createEntity();
    // m_ecsCoordinator->addComponent<Transform>( z_axis_line, primitive_transform );
    // m_ecsCoordinator->addComponent<Renderable>( z_axis_line,
    //                                             Renderable( glm::vec3( 0.157f, 0.208f, 0.576f ) )
    //                                             );
    // m_ecsCoordinator->addComponent<Primitive>( z_axis_line, primitive );
  }

  void Platformer::draw( f32 deltaTime ) noexcept {

    m_fontRenderer->renderText( "this is working text", 5.0f, 5.0f, 1.0f,
                                glm::vec3( 0.0f, 0.902, 0.463 ) );
  }


  void Platformer::update( f32 deltaTime ) noexcept {

    this->processMouseEvents( deltaTime );
    this->processKeyboardEvents( deltaTime );
    this->processMouseScroll( deltaTime );

    auto &camera_component = m_ecsCoordinator->getComponent<CameraComponent>( m_cameraEntity );

    auto &c_transform = m_ecsCoordinator->getComponent<Transform>( m_cameraEntity );
    auto &c_camera = m_ecsCoordinator->getComponent<CameraComponent>( m_cameraEntity );


    glm::mat4 view =
        Math::lookAt( c_transform.position, c_transform.position + camera_component.cameraFront,
                      camera_component.cameraUp );
    glm::mat4 projection = glm::perspective( glm::radians( c_camera.fieldOfView ),
                                             static_cast<f32>( m_settings->getWidth() ) /
                                                 static_cast<f32>( m_settings->getHeight() ),
                                             c_camera.near, c_camera.far );

    // We set projection matrix to the object that are "moving"
    // since the camera is static, and we shift the world
    m_assetManager->getAsset<ShaderSet>( "sprite_shader" )
        ->use()
        .SetMatrix4( "projection", projection );

    m_assetManager->getAsset<ShaderSet>( "sprite_shader" )->use().SetMatrix4( "view", view );

    m_assetManager->getAsset<ShaderSet>( "line_shader" )
        ->use()
        .SetMatrix4( "projection", projection );

    m_assetManager->getAsset<ShaderSet>( "model_shader" )
        ->use()
        .SetMatrix4( "projection", projection );

    m_assetManager->getAsset<ShaderSet>( "model_shader" )->use().SetMatrix4( "view", view );
  }


  void Platformer::test3d() noexcept {

    auto entity = m_ecsCoordinator->createEntity();

    Transform transform;
    transform.position = glm::vec3( 0.0f, 0.0f, -4.0f );
    transform.rotation = 45.0f;
    m_ecsCoordinator->addComponent<Transform>( entity, transform );

    Renderable renderable;
    renderable.color = glm::vec3( 1.0f, 1.0f, 1.0f );
    m_ecsCoordinator->addComponent<Renderable>( entity, renderable );

    // MeshComponent mesh;
    // mesh.texture = m_assetManager->getAsset<Texture2D>( "container" );
    // m_ecsCoordinator->addComponent<MeshComponent>( entity, mesh );
  }

  void Platformer::processKeyboardEvents( f32 dt ) noexcept {

    constexpr f32 movement_speed = 0.010f;
    f32 velocity = movement_speed * dt;

    // Camera components
    auto &c_transform = m_ecsCoordinator->getComponent<Transform>( m_cameraEntity );
    auto &c_camera = m_ecsCoordinator->getComponent<CameraComponent>( m_cameraEntity );

    if ( m_inputManager->isKeyHoldDown( SDLK_w ) ) {
      // Forward
      c_transform.position += c_camera.cameraFront * velocity;
    }

    if ( m_inputManager->isKeyHoldDown( SDLK_s ) ) {
      // Backward
      c_transform.position -= c_camera.cameraFront * velocity;
    }

    if ( m_inputManager->isKeyHoldDown( SDLK_a ) ) {
      // Left
      c_transform.position -= c_camera.cameraRight * velocity;
    }

    if ( m_inputManager->isKeyHoldDown( SDLK_d ) ) {
      // Right
      c_transform.position += c_camera.cameraRight * velocity;
    }


    if ( m_inputManager->isKeyPressed( SDLK_ESCAPE ) ) {
      m_inputManager->terminateEngine();
    }
  }


  void Platformer::processMouseEvents( f32 dt ) noexcept {

    auto &c_camera = m_ecsCoordinator->getComponent<CameraComponent>( m_cameraEntity );

    if ( m_inputManager->mouseLeftPressed() ) {

      if ( m_firstMouse ) {
        m_lastX = m_inputManager->getMousePos().x;
        m_lastY = m_inputManager->getMousePos().y;
        m_firstMouse = false;
      }

      float xoffset = m_inputManager->getMousePos().x - m_lastX;
      float yoffset = m_lastY - m_inputManager->getMousePos().y;

      f32 sensitivity = 0.004f;

      xoffset *= sensitivity;
      yoffset *= sensitivity;

      c_camera.yaw += xoffset;
      c_camera.pitch += yoffset;

      if ( c_camera.pitch > 89.0f )
        c_camera.pitch = 89.0f;
      if ( c_camera.pitch < -89.0f )
        c_camera.pitch = -89.0f;

      c_camera.shouldCameraUpdate = true;
    }
  }

  void Platformer::processMouseScroll( f32 dt ) noexcept {

    auto &c_camera = m_ecsCoordinator->getComponent<CameraComponent>( m_cameraEntity );

    if ( c_camera.fieldOfView >= 1.0f && c_camera.fieldOfView <= 45.0f ) {
      c_camera.fieldOfView -= m_inputManager->getVerticalWheel() * 0.4f;
    }

    if ( c_camera.fieldOfView <= 1.0f ) {
      c_camera.fieldOfView = 1.0f;
    }

    if ( c_camera.fieldOfView >= 45.0f ) {
      c_camera.fieldOfView = 45.0f;
    }
  }

  void Platformer::testModel() noexcept {

    auto crate_model =
        m_assetManager->createBuilder<Model>( m_assetManager )
            .setModelPath( FileSystem::getPath( "assets/models/container/container.obj" ) )
            .build();

    m_assetManager->storeAsset<Model>( "crate_model", crate_model );


    auto model = m_assetManager->getAsset<Model>( "crate_model" );
    auto meshes = model->meshes;

    for ( u32 row = 0; row < 6; row++ ) {
      for ( u32 col = 0; col < 6; col++ ) {

        u32 k = static_cast<f32>( row * 10.0f );
        u32 m = static_cast<f32>( col * 10.0f );

        for ( const auto &i : meshes ) {
          auto entity = m_ecsCoordinator->createEntity();
          Transform transform;
          transform.position = glm::vec3( k, 0.0f, m );
          transform.scale = glm::vec3( 0.2f );

          MeshComponent mesh;
          mesh.vertices = i.verticies;
          mesh.textures = i.textures;
          mesh.indices = i.indices;

          Renderable renderable;
          renderable.color = glm::vec3( 1.0f, 1.0f, 1.0f );

          m_ecsCoordinator->addComponent<Transform>( entity, transform );
          m_ecsCoordinator->addComponent<Renderable>( entity, renderable );
          m_ecsCoordinator->addComponent<MeshComponent>( entity, mesh );
        }
      }
    }
  }

}    // namespace platformer
