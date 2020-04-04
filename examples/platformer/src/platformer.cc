#include "platformer.hh"

#include <Nile/asset/builder/model_builder.hh>
#include <Nile/asset/builder/shaderset_builder.hh>
#include <Nile/asset/subsystem/texture_loader.hh>
#include <Nile/core/assert.hh>
#include <Nile/core/file_system.hh>
#include <Nile/debug/benchmark_timer.hh>
#include <Nile/ecs/components/camera_component.hh>
#include <Nile/ecs/components/font_component.hh>
#include <Nile/ecs/components/mesh_component.hh>
#include <Nile/ecs/components/primitive.hh>
#include <Nile/ecs/components/relationship.hh>
#include <Nile/ecs/components/renderable.hh>
#include <Nile/ecs/components/sprite.hh>
#include <Nile/ecs/components/transform.hh>
#include <Nile/log/log.hh>
#include <Nile/math/utils.hh>
#include <Nile/renderer/font.hh>
#include <Nile/renderer/model.hh>
#include <Nile/renderer/texture2d.hh>

#include <cstdio>

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

    log::notice( "Platformer::initialize()\n" );

    // auto modelShader =
    //     m_assetManager->createBuilder<ShaderSet>()
    //         .setVertexPath( FileSystem::getPath( "assets/shaders/model.vert.glsl" ) )
    //         .setFragmentPath( FileSystem::getPath( "assets/shaders/model.frag.glsl" ) )
    //         .build();
    //
    //    m_assetManager->storeAsset<ShaderSet>( "model_shader", modelShader );

    // auto lampShader =
    //     m_assetManager->createBuilder<ShaderSet>()
    //         .setVertexPath( FileSystem::getPath( "assets/shaders/lamp_vertex.glsl" ) )
    //         .setFragmentPath( FileSystem::getPath( "assets/shaders/lamp_fragment.glsl" ) )
    //         .build();
    //
    //  m_assetManager->storeAsset<ShaderSet>( "lamp_shader", lampShader );

    // Load some shaders
    // m_lampShader = m_assetManager->getAsset<ShaderSet>( "lamp_shader" );
    // m_modelShader = m_assetManager->getAsset<ShaderSet>( "model_shader" );
    // m_lineShader = m_assetManager->getAsset<ShaderSet>( "line_shader" );
    //
    // ASSERT_M( m_lampShader, "lamp shader is null\n" );
    // ASSERT_M( m_modelShader, "model shader is null\n" );
    // ASSERT_M( m_lineShader, "line shader is null\n" );
    //
    // m_cameraEntity = m_ecsCoordinator->createEntity();
    //
    // Transform camera_transform;
    // camera_transform.position = glm::vec3( -12.0f, 20.0f, 70.0f );
    // m_ecsCoordinator->addComponent<Transform>( m_cameraEntity, camera_transform );
    //
    // CameraComponent cameraComponent( 0.1f, 200.0f, 45.0f, ProjectionType::PERSPECTIVE );
    // m_ecsCoordinator->addComponent<CameraComponent>( m_cameraEntity, cameraComponent );
    //
    // m_assetManagerHelper = std::make_shared<AssetManagerHelper>( m_assetManager );

    //    m_textBuffer = std::make_unique<TextBuffer>( m_settings, m_ecsCoordinator, m_assetManager
    //    );

    //    this->drawTextureFloor();
    this->drawStoneTiles();
    //  this->drawContainers();
    //    this->drawNanoModel();
    //   this->drawGrass();
    //   this->drawWindows();
    // this->drawFont();
    //    this->drawLigths();

    // m_textBuffer->append( "this is text 1", glm::vec2( 40, 20 ) );
    // m_textBuffer->append( "this is text 2", glm::vec2( 40, 60 ) );
    // m_textBuffer->append( "this is text 3", glm::vec2( 40, 100 ) );
    // m_textBuffer->append( "this is text 4", glm::vec2( 80, 100 ) );
  }

  void Platformer::draw( f32 deltaTime ) noexcept {}

  void Platformer::update( f32 deltaTime ) noexcept {

    this->processMouseEvents( deltaTime );
    this->processKeyboardEvents( deltaTime );
    this->processMouseScroll( deltaTime );

    // m_textBuffer->update( deltaTime );
    //
    // auto &camera_component = m_ecsCoordinator->getComponent<CameraComponent>( m_cameraEntity );
    //
    // auto &c_transform = m_ecsCoordinator->getComponent<Transform>( m_cameraEntity );
    // auto &c_camera = m_ecsCoordinator->getComponent<CameraComponent>( m_cameraEntity );
    //
    //
    // glm::mat4 view =
    //     Math::lookAt( c_transform.position, c_transform.position + camera_component.cameraFront,
    //                   camera_component.cameraUp );
    // glm::mat4 projection = glm::perspective( glm::radians( c_camera.fieldOfView ),
    //                                          static_cast<f32>( m_settings->getWidth() ) /
    //                                              static_cast<f32>( m_settings->getHeight() ),
    //                                          c_camera.near, c_camera.far );
    //
    // // We set projection matrix to the object that are "moving"
    // // since the camera is static, and we shift the world
    //
    // m_lineShader->use().SetMatrix4( "projection", projection );
    //
    // m_modelShader->use();
    // m_modelShader->SetMatrix4( "view", view );
    // m_modelShader->SetMatrix4( "projection", projection );
    // m_modelShader->SetVector3f( "lightColor", lightColor );
    // m_modelShader->SetFloat( "material.shininess", 32.0f );
    // m_modelShader->SetVector3f( "pointLight.position", lightPos );
    // m_modelShader->SetVector3f( "pointLight.ambient", 0.2f, 0.2f, 0.2f );
    // m_modelShader->SetVector3f( "pointLight.diffuse", 0.7f, 0.7f, 0.7f );
    // m_modelShader->SetVector3f( "pointLight.specular", 0.6f, 0.6f, 0.6f );
    // m_modelShader->SetFloat( "pointLight.constant", 1.0f );
    // m_modelShader->SetFloat( "pointLight.linear", 0.045f );
    // m_modelShader->SetFloat( "pointLight.quadratic", 0.0075f );
    //
    // m_modelShader->SetVector3f( "directionalLight.direction", -0.2f, -1.0f, -0.3f );
    // m_modelShader->SetVector3f( "directionalLight.ambient", 0.05f, 0.05f, 0.05f );
    // m_modelShader->SetVector3f( "directionalLight.diffuse", 0.4f, 0.4f, 0.4f );
    // m_modelShader->SetVector3f( "directionalLight.specular", 0.5f, 0.5f, 0.5f );
    //
    // m_modelShader->SetVector3f( "viewPos", c_transform.position );
    //
    // m_lampShader->use().SetMatrix4( "projection", projection );
    // m_lampShader->use().SetMatrix4( "view", view );
    //
    // char buffer[ 32 ];
    // sprintf( buffer, "@fps: %.3f", ( 1000 / deltaTime ) );
    //
    // m_screenText->print( buffer, TextPosition::LEFT_UP );
    //
    // glCheckError();
  }


  void Platformer::processKeyboardEvents( f32 dt ) noexcept {

    constexpr f32 movement_speed = 0.010f;
    f32 velocity = movement_speed * dt;

    // Camera components
    auto &c_transform = m_ecsCoordinator->getComponent<Transform>( m_cameraEntity );
    auto &c_camera = m_ecsCoordinator->getComponent<CameraComponent>( m_cameraEntity );

    if ( m_inputManager->isKeyPressed( SDLK_o ) ) {
      m_textBuffer->append( "random text", glm::vec2( 40, 10 ) );
    }

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

  void Platformer::drawStoneTiles() noexcept {

    log::notice( "Platformer::drawStoneTiles()\n" );

    auto model = m_assetManager->storeAsset<Model>(
        "stonetile_model",
        m_assetManager->createBuilder<Model>( m_assetManager )
            .setModelPath( FileSystem::getPath( "assets/models/stonetile/stonetile.obj" ) )
            .build() );

    auto stonetile_mesh = model->meshes;

    const f32 x_model_offset = 11.333f;
    const f32 z_model_offset = 11.333f;

    for ( u32 row = 0; row < 1; row++ ) {
      for ( u32 col = 0; col < 1; col++ ) {

        u32 x = static_cast<f32>( row * x_model_offset );
        u32 z = static_cast<f32>( col * z_model_offset );

        for ( const auto &i : stonetile_mesh ) {
          auto entity = m_ecsCoordinator->createEntity();
          Transform transform;
          transform.position = glm::vec3( x, 0.0f, z );
          transform.scale = glm::vec3( 3.0f );

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

  void Platformer::drawTextureFloor() noexcept {

    auto floor_texture = m_assetManager->loadAsset<Texture2D>(
        "metal_texture", FileSystem::getPath( "assets/textures/metal.png" ) );

    auto entity = m_ecsCoordinator->createEntity();

    Transform transform( glm::vec3( 0.0f, 0.0f, 0.0f ), glm::vec3( 120.f ) );

    transform.xRotation = 90.0f;

    Renderable renderable( glm::vec3( 1.0f ) );

    // Background
    m_ecsCoordinator->addComponent<Transform>( entity, transform );
    m_ecsCoordinator->addComponent<Renderable>( entity, renderable );
    m_ecsCoordinator->addComponent<SpriteComponent>( entity, SpriteComponent( floor_texture ) );
  }

  void Platformer::drawContainers() noexcept {

    auto model = m_assetManager->storeAsset<Model>(
        "container_model",
        m_assetManager->createBuilder<Model>( m_assetManager )
            .setModelPath( FileSystem::getPath( "assets/models/container/container.obj" ) )
            .build() );

    auto model_mesh = model->meshes;

    for ( i32 j = 0; j < 4; j++ ) {

      const f32 offset_x = 12.0f;
      const f32 offset_z = 8.0f;


      for ( const auto &i : model_mesh ) {

        auto entity = m_ecsCoordinator->createEntity();
        Transform transform;
        transform.position = glm::vec3( j * offset_x + 20, 3.2f, j * offset_z + 20 );
        transform.scale = glm::vec3( 0.15f );

        transform.xRotation = 90.0f;
        transform.yRotation = 90.0f;

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

  void Platformer::drawGrass() noexcept {

    auto grass_texture = m_assetManager->loadAsset<Texture2D>(
        "grass", FileSystem::getPath( "assets/textures/grass.png" ) );


    grass_texture->setParameter( nile::TextureTargetParams::TEXTURE_WRAP_S,
                                 nile::TextureParams::CLAMP_TO_EDGE );
    grass_texture->setParameter( nile::TextureTargetParams::TEXTURE_WRAP_T,
                                 nile::TextureParams::CLAMP_TO_EDGE );

    const i32 offset_x = 60;
    const i32 offset_z = 20;

    std::vector<glm::vec3> vegetation;
    vegetation.emplace_back( -1.5f + offset_x, 0.0f, -2.48f + offset_z );
    vegetation.emplace_back( 5.5f + offset_x, 0.0f, 0.51f + offset_z );
    vegetation.emplace_back( 0.0f + offset_x, 0.0f, 4.7f + offset_z );
    vegetation.emplace_back( -6.3f + offset_x, 0.0f, -8.3f + offset_z );
    vegetation.emplace_back( 12.5f + offset_x, 0.0f, -0.6f + offset_z );

    for ( const auto &i : vegetation ) {
      m_testEntity = m_ecsCoordinator->createEntity();

      Transform transform( i, glm::vec3( 8.0f ) );

      Renderable renderable( glm::vec3( 1.0f ) );

      // Background
      m_ecsCoordinator->addComponent<Transform>( m_testEntity, transform );
      m_ecsCoordinator->addComponent<Renderable>( m_testEntity, renderable );
      m_ecsCoordinator->addComponent<SpriteComponent>( m_testEntity,
                                                       SpriteComponent( grass_texture ) );
    }
  }

  void Platformer::drawWindows() noexcept {

    auto window_texture = m_assetManager->loadAsset<Texture2D>(
        "window", FileSystem::getPath( "assets/textures/window.png" ) );

    log::print( "count: %d\n", window_texture->getRefCount() );

    auto camera_transform = m_ecsCoordinator->getComponent<Transform>( m_cameraEntity );

    const i32 offset_x = 20;
    const i32 offset_z = 10;

    std::vector<glm::vec3> windows;
    windows.emplace_back( -1.5f + offset_x, 0.0f, -1.48f + offset_z );
    windows.emplace_back( 2.5f + offset_x, 0.0f, 2.51f + offset_z );
    windows.emplace_back( 0.0f + offset_x, 0.0f, 4.7f + offset_z );
    windows.emplace_back( -6.3f + offset_x, 0.0f, -12.3f + offset_z );
    windows.emplace_back( 7.5f + offset_x, 0.0f, -0.6f + offset_z );

    std::map<f32, glm::vec3> sorted;

    for ( u32 i = 0; i < 5; i++ ) {
      f32 distance = glm::length( camera_transform.position - windows[ i ] );
      sorted[ distance ] = windows[ i ];
    }

    for ( const auto &[ first, second ] : sorted ) {
      auto entity = m_ecsCoordinator->createEntity();

      Transform transform( second, glm::vec3( 4.0f ) );

      Renderable renderable( glm::vec3( 1.0f ) );

      // Background
      m_ecsCoordinator->addComponent<Transform>( entity, transform );
      m_ecsCoordinator->addComponent<Renderable>( entity, renderable );
      m_ecsCoordinator->addComponent<SpriteComponent>( entity, SpriteComponent( window_texture ) );
    }
  }

  void Platformer::drawNanoModel() noexcept {

    BenchmarkTimer timer( "drawNanoModel()" );

    auto model = m_assetManager->storeAsset<Model>(
        "nanosuit",
        m_assetManager->createBuilder<Model>( m_assetManager )
            .setModelPath( FileSystem::getPath( "assets/models/nanosuit/nanosuit.obj" ) )
            .build() );

    auto model_mesh = model->meshes;

    m_nanoModelEntity = m_ecsCoordinator->createEntity();
    m_ecsCoordinator->addComponent<Relationship>( m_nanoModelEntity, Relationship() );

    for ( const auto &i : model_mesh ) {

      auto entity = m_ecsCoordinator->createEntity();

      m_ecsCoordinator->addComponent<Relationship>( entity, Relationship() );
      m_ecsCoordinator->attachTo( m_nanoModelEntity, entity );

      Transform transform;
      transform.position = glm::vec3( 6.0f, 0.0f, 22.0f );
      transform.yRotation = -45.0f;
      transform.scale = glm::vec3( 1.0f );

      MeshComponent mesh;
      mesh.vertices = i.verticies;
      mesh.textures = i.textures;
      mesh.indices = i.indices;

      Renderable renderable;
      renderable.color = glm::vec3( 1.0f, 1.0f, 1.0f );
      renderable.blend = false;


      m_ecsCoordinator->addComponent<Transform>( entity, transform );
      m_ecsCoordinator->addComponent<Renderable>( entity, renderable );
      m_ecsCoordinator->addComponent<MeshComponent>( entity, mesh );
    }
  }

  void Platformer::drawFont() noexcept {

    BenchmarkTimer timer( "drawFont()" );

    auto sfmono_font = m_assetManager->loadAsset<Font>(
        "sfmono_font", FileSystem::getPath( "assets/fonts/LiberationMono-Regular.ttf" ) );

    auto entity = m_ecsCoordinator->createEntity();

    FontComponent font;
    font.font = sfmono_font;
    font.text = "This is testing font";

    Renderable renderable;
    renderable.color = glm::vec3( 0.9f, 0.2f, 0.2f );

    Transform transform;
    transform.position = glm::vec3( 22.0f, 0.0f, 0.0f );
    transform.scale = glm::vec3( 1.0f );

    m_ecsCoordinator->addComponent<Transform>( entity, transform );
    m_ecsCoordinator->addComponent<Renderable>( entity, renderable );
    m_ecsCoordinator->addComponent<FontComponent>( entity, font );
  }

  void Platformer::drawLigths() noexcept {

    BenchmarkTimer timer( "drawLights()" );

    auto model = m_assetManager->storeAsset<Model>(
        "light_sphere",
        m_assetManager->createBuilder<Model>( m_assetManager )
            .setModelPath( FileSystem::getPath( "assets/models/sphere/sphere.obj" ) )
            .build() );

    auto model_mesh = model->meshes;

    for ( const auto &i : model_mesh ) {

      m_lampEntity = m_ecsCoordinator->createEntity();

      Transform transform;
      transform.position = lightPos;
      transform.scale = glm::vec3( 1.0f );

      MeshComponent mesh;
      mesh.vertices = i.verticies;
      mesh.textures = i.textures;
      mesh.indices = i.indices;

      Renderable renderable;
      renderable.color = glm::vec3( 1.0f, 1.0f, 1.0f );
      renderable.blend = false;
      renderable.shaderSet = m_assetManager->getAsset<ShaderSet>( "lamp_shader" );

      m_ecsCoordinator->addComponent<Transform>( m_lampEntity, transform );
      m_ecsCoordinator->addComponent<Renderable>( m_lampEntity, renderable );
      m_ecsCoordinator->addComponent<MeshComponent>( m_lampEntity, mesh );
    }
  }

}    // namespace platformer
