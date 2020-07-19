#include "platformer.hh"
#include "Nile/core/types.hh"

#include <Nile/asset/builder/model_builder.hh>
#include <Nile/asset/builder/shaderset_builder.hh>
#include <Nile/asset/subsystem/texture_loader.hh>
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
      : game_host_( gameHost )
      , input_manager_( gameHost->get_input_manager() )
      , renderer_( gameHost->get_renderer() )
      , settings_( gameHost->get_settings() )
      , assets_manager_( gameHost->get_asset_manager() )
      , ecs_coordinator_( gameHost->get_ecs_coordinator() ) {

    last_x_ = settings_->getWidth() / 2.0f;
    last_y_ = settings_->getHeight() / 2.0f;
  }

  void Platformer::initialize() noexcept {

    // auto model_shader =
    //     assets_manager_->createBuilder<ShaderSet>()
    //         .setVertexPath( FileSystem::getPath( "assets/shaders/model.vert.glsl" ) )
    //         .setFragmentPath( FileSystem::getPath( "assets/shaders/model.frag.glsl" ) )
    //         .build();
    //
    //    assets_manager_->storeAsset<ShaderSet>( "model_shader", model_shader );

    auto lampShader =
        assets_manager_->createBuilder<ShaderSet>()
            .setVertexPath( FileSystem::getPath( "assets/shaders/lamp_vertex.glsl" ) )
            .setFragmentPath( FileSystem::getPath( "assets/shaders/lamp_fragment.glsl" ) )
            .build();

    assets_manager_->storeAsset<ShaderSet>( "lamp_shader", lampShader );

    camera_entity_ = ecs_coordinator_->createEntity();

    Transform camera_transform;
    camera_transform.position = glm::vec3( -12.0f, 20.0f, 70.0f );
    ecs_coordinator_->addComponent<Transform>( camera_entity_, camera_transform );

    CameraComponent camera_component( 0.1f, 200.0f, 45.0f, ProjectionType::PERSPECTIVE );
    ecs_coordinator_->addComponent<CameraComponent>( camera_entity_, camera_component );

    // assets_manager_Helper = std::make_shared<AssetManagerHelper>( assets_manager_ );

    text_buffer_ = std::make_unique<TextBuffer>( settings_, ecs_coordinator_, assets_manager_ );

    this->draw_textured_floor();
    // this->draw_stone_tiles();
    //    this->draw_containers();
    // this->draw_nano_model();
    // this->draw_grass();
    // this->draw_windows();
    // this->draw_text_font();
    this->draw_micro_subscene();
    // this->draw_point_lights();
    // this->draw_pizza_box();
    // this->draw_sprites_test();

    // m_textBuffer->append( "this is text 1", glm::vec2( 40, 20 ) );
    // m_textBuffer->append( "this is text 2", glm::vec2( 40, 60 ) );
    // m_textBuffer->append( "this is text 3", glm::vec2( 40, 100 ) );
    // m_textBuffer->append( "this is text 4", glm::vec2( 80, 100 ) );
  }

  void Platformer::draw( f32 deltaTime ) noexcept {}

  void Platformer::update( f32 deltaTime ) noexcept {

    this->process_mouse_events( deltaTime );
    this->process_keyboard_events( deltaTime );
    this->process_mouse_scrolling_events( deltaTime );

    text_buffer_->update( deltaTime );

    auto &camera_component = ecs_coordinator_->getComponent<CameraComponent>( camera_entity_ );
    auto &c_transform = ecs_coordinator_->getComponent<Transform>( camera_entity_ );
    auto &c_camera = ecs_coordinator_->getComponent<CameraComponent>( camera_entity_ );


    glm::mat4 view =
        Math::lookAt( c_transform.position, c_transform.position + camera_component.cameraFront,
                      camera_component.cameraUp );
    glm::mat4 projection = glm::perspective( glm::radians( c_camera.fieldOfView ),
                                             static_cast<f32>( settings_->getWidth() ) /
                                                 static_cast<f32>( settings_->getHeight() ),
                                             c_camera.near, c_camera.far );

    // We set projection matrix to the object that are "moving"
    // since the camera is static, and we shift the world

    assets_manager_->getAsset<ShaderSet>( "line_shader" )
        ->use()
        .SetMatrix4( "projection", projection );

    auto model_shader = assets_manager_->getAsset<ShaderSet>( "model_shader" );
    model_shader->use();
    model_shader->SetMatrix4( "view", view );
    model_shader->SetMatrix4( "projection", projection );
    model_shader->SetVector3f( "lightColor", default_light_color_ );
    model_shader->SetFloat( "material.shininess", 32.0f );

    // Point light 1
    model_shader->SetVector3f( "pointLights[0].position", point_lights_positions_[ 0 ] );
    model_shader->SetVector3f( "pointLights[0].ambient", 0.2f, 0.2f, 0.2f );
    model_shader->SetVector3f( "pointLights[0].diffuse", 0.7f, 0.7f, 0.7f );
    model_shader->SetVector3f( "pointLights[0].specular", 0.6f, 0.6f, 0.6f );
    model_shader->SetFloat( "pointLights[0].constant", 1.0f );
    model_shader->SetFloat( "pointLights[0].linear", 0.045f );
    model_shader->SetFloat( "pointLights[0].quadratic", 0.0075f );

    // Point light 2
    model_shader->SetVector3f( "pointLights[1].position", point_lights_positions_[ 1 ] );
    model_shader->SetVector3f( "pointLights[1].ambient", 0.2f, 0.2f, 0.2f );
    model_shader->SetVector3f( "pointLights[1].diffuse", 0.7f, 0.7f, 0.7f );
    model_shader->SetVector3f( "pointLights[1].specular", 0.6f, 0.6f, 0.6f );
    model_shader->SetFloat( "pointLights[1].constant", 1.0f );
    model_shader->SetFloat( "pointLights[1].linear", 0.045f );
    model_shader->SetFloat( "pointLights[1].quadratic", 0.0075f );

    // Point light 3
    model_shader->SetVector3f( "pointLights[2].position", point_lights_positions_[ 2 ] );
    model_shader->SetVector3f( "pointLights[2].ambient", 0.2f, 0.2f, 0.2f );
    model_shader->SetVector3f( "pointLights[2].diffuse", 0.7f, 0.7f, 0.7f );
    model_shader->SetVector3f( "pointLights[2].specular", 0.6f, 0.6f, 0.6f );
    model_shader->SetFloat( "pointLights[2].constant", 1.0f );
    model_shader->SetFloat( "pointLights[2].linear", 0.045f );
    model_shader->SetFloat( "pointLights[2].quadratic", 0.0075f );

    // Point light 4
    model_shader->SetVector3f( "pointLights[3].position", point_lights_positions_[ 3 ] );
    model_shader->SetVector3f( "pointLights[3].ambient", 0.2f, 0.2f, 0.2f );
    model_shader->SetVector3f( "pointLights[3].diffuse", 0.7f, 0.7f, 0.7f );
    model_shader->SetVector3f( "pointLights[3].specular", 0.6f, 0.6f, 0.6f );
    model_shader->SetFloat( "pointLights[3].constant", 1.0f );
    model_shader->SetFloat( "pointLights[3].linear", 0.045f );
    model_shader->SetFloat( "pointLights[3].quadratic", 0.0075f );

    // Point light 5
    model_shader->SetVector3f( "pointLights[4].position", point_lights_positions_[ 4 ] );
    model_shader->SetVector3f( "pointLight[4].ambient", 0.2f, 0.2f, 0.2f );
    model_shader->SetVector3f( "pointLight[4].diffuse", 0.2f, 0.2f, 0.2f );
    model_shader->SetVector3f( "pointLight[4].specular", 0.6f, 0.6f, 0.6f );
    model_shader->SetFloat( "pointLight[4].constant", 1.0f );
    model_shader->SetFloat( "pointLight[4].linear", 0.045f );
    model_shader->SetFloat( "pointLight[4].quadratic", 0.0075f );

    model_shader->SetVector3f( "directionalLight.direction", -0.2f, -1.0f, -0.3f );
    model_shader->SetVector3f( "directionalLight.ambient", 0.02f, 0.02f, 0.02f );
    model_shader->SetVector3f( "directionalLight.diffuse", 0.1f, 0.1f, 0.1f );
    model_shader->SetVector3f( "directionalLight.specular", 0.5f, 0.5f, 0.5f );

    model_shader->SetVector3f( "viewPos", c_transform.position );

    assets_manager_->getAsset<ShaderSet>( "lamp_shader" )
        ->use()
        .SetMatrix4( "projection", projection );

    assets_manager_->getAsset<ShaderSet>( "lamp_shader" )->use().SetMatrix4( "view", view );

    // char buffer[ 32 ];
    // sprintf( buffer, "@fps: %.3f", ( 1000 / deltaTime ) );
    //
    // m_screenText->print( buffer, TextPosition::LEFT_UP );
    //
    glCheckError();
  }


  void Platformer::process_keyboard_events( f32 dt ) noexcept {

    constexpr f32 movement_speed = 0.010f;
    f32 velocity = movement_speed * dt;

    // Camera components
    auto &c_transform = ecs_coordinator_->getComponent<Transform>( camera_entity_ );
    auto &c_camera = ecs_coordinator_->getComponent<CameraComponent>( camera_entity_ );

    if ( input_manager_->isKeyPressed( SDLK_o ) ) {
      text_buffer_->append( "random text", glm::vec2( 40, 10 ) );
    }

    if ( input_manager_->isKeyHoldDown( SDLK_w ) ) {
      // Forward
      c_transform.position += c_camera.cameraFront * velocity;
    }

    if ( input_manager_->isKeyHoldDown( SDLK_s ) ) {
      // Backward
      c_transform.position -= c_camera.cameraFront * velocity;
    }

    if ( input_manager_->isKeyHoldDown( SDLK_a ) ) {
      // Left
      c_transform.position -= c_camera.cameraRight * velocity;
    }

    if ( input_manager_->isKeyHoldDown( SDLK_d ) ) {
      // Right
      c_transform.position += c_camera.cameraRight * velocity;
    }

    if ( input_manager_->isKeyPressed( SDLK_ESCAPE ) ) {
      input_manager_->terminateEngine();
    }
  }

  void Platformer::process_mouse_events( f32 dt ) noexcept {

    auto &c_camera = ecs_coordinator_->getComponent<CameraComponent>( camera_entity_ );

    if ( input_manager_->mouseLeftPressed() ) {

      if ( first_mouse_entry_ ) {
        last_x_ = input_manager_->getMousePos().x;
        last_y_ = input_manager_->getMousePos().y;
        first_mouse_entry_ = false;
      }

      float xoffset = input_manager_->getMousePos().x - last_x_;
      float yoffset = last_y_ - input_manager_->getMousePos().y;

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

  void Platformer::process_mouse_scrolling_events( f32 dt ) noexcept {

    auto &c_camera = ecs_coordinator_->getComponent<CameraComponent>( camera_entity_ );

    if ( c_camera.fieldOfView >= 1.0f && c_camera.fieldOfView <= 45.0f ) {
      c_camera.fieldOfView -= input_manager_->getVerticalWheel() * 0.4f;
    }

    if ( c_camera.fieldOfView <= 1.0f ) {
      c_camera.fieldOfView = 1.0f;
    }

    if ( c_camera.fieldOfView >= 45.0f ) {
      c_camera.fieldOfView = 45.0f;
    }
  }

  void Platformer::draw_stone_tiles() noexcept {

    BenchmarkTimer timer( "draw_stone_tiles()" );

    auto model = assets_manager_->storeAsset<Model>(
        "stonetile_model",
        assets_manager_->createBuilder<Model>( assets_manager_ )
            .setModelPath( FileSystem::getPath( "assets/models/stonetile/stonetile.obj" ) )
            .build() );

    auto stonetile_mesh = model->meshes;

    const f32 x_model_offset = 11.333f;
    const f32 z_model_offset = 11.333f;

    for ( u32 row = 0; row < 8; row++ ) {
      for ( u32 col = 0; col < 8; col++ ) {

        u32 x = static_cast<f32>( row * x_model_offset );
        u32 z = static_cast<f32>( col * z_model_offset );

        for ( const auto &i : stonetile_mesh ) {
          auto entity = ecs_coordinator_->createEntity();
          Transform transform;
          transform.position = glm::vec3( x, 0.0f, z );
          transform.scale = glm::vec3( 3.0f );

          MeshComponent mesh;
          mesh.vertices = i.verticies;
          mesh.textures = i.textures;
          mesh.indices = i.indices;

          Renderable renderable;
          renderable.color = glm::vec3( 1.0f, 1.0f, 1.0f );

          ecs_coordinator_->addComponent<Transform>( entity, transform );
          ecs_coordinator_->addComponent<Renderable>( entity, renderable );
          ecs_coordinator_->addComponent<MeshComponent>( entity, mesh );
        }
      }
    }
  }

  void Platformer::draw_textured_floor() noexcept {

    BenchmarkTimer timer( "draw_textured_floor()" );

    auto floor_texture = assets_manager_->loadAsset<Texture2D>(
        "metal_texture", FileSystem::getPath( "assets/textures/metal.png" ) );

    auto entity = ecs_coordinator_->createEntity();

    Transform transform( glm::vec3( 0.0f, 0.0f, 0.0f ), glm::vec3( 120.f ) );
    transform.xRotation = 90.0f;

    Renderable renderable( glm::vec3( 1.0f ) );

    // Background
    ecs_coordinator_->addComponent<Transform>( entity, transform );
    ecs_coordinator_->addComponent<Renderable>( entity, renderable );
    ecs_coordinator_->addComponent<SpriteComponent>( entity, SpriteComponent( floor_texture ) );
  }

  void Platformer::draw_containers() noexcept {

    auto model = assets_manager_->storeAsset<Model>(
        "container_model",
        assets_manager_->createBuilder<Model>( assets_manager_ )
            .setModelPath( FileSystem::getPath( "assets/models/container/container.obj" ) )
            .build() );

    auto model_mesh = model->meshes;

    for ( i32 j = 0; j < 4; j++ ) {

      const f32 offset_x = 12.0f;
      const f32 offset_z = 8.0f;

      for ( const auto &i : model_mesh ) {

        auto entity = ecs_coordinator_->createEntity();
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
        ecs_coordinator_->addComponent<Transform>( entity, transform );
        ecs_coordinator_->addComponent<Renderable>( entity, renderable );
        ecs_coordinator_->addComponent<MeshComponent>( entity, mesh );
      }
    }
  }

  void Platformer::draw_grass() noexcept {

    BenchmarkTimer timer( "draw_grass()" );

    auto grass_texture = assets_manager_->loadAsset<Texture2D>(
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
      test_entity_ = ecs_coordinator_->createEntity();

      Transform transform( i, glm::vec3( 8.0f ) );
      Renderable renderable( glm::vec3( 1.0f ) );

      // Background
      ecs_coordinator_->addComponent<Transform>( test_entity_, transform );
      ecs_coordinator_->addComponent<Renderable>( test_entity_, renderable );
      ecs_coordinator_->addComponent<SpriteComponent>( test_entity_,
                                                      SpriteComponent( grass_texture ) );
    }
  }

  void Platformer::draw_windows() noexcept {

    BenchmarkTimer timer( "draw_windows()" );

    auto window_texture = assets_manager_->loadAsset<Texture2D>(
        "window", FileSystem::getPath( "assets/textures/window.png" ) );

    auto camera_transform = ecs_coordinator_->getComponent<Transform>( camera_entity_ );

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
      auto entity = ecs_coordinator_->createEntity();
      Transform transform( second, glm::vec3( 4.0f ) );
      Renderable renderable( glm::vec3( 1.0f ) );

      renderable.blend = false;
      // Background
      ecs_coordinator_->addComponent<Transform>( entity, transform );
      ecs_coordinator_->addComponent<Renderable>( entity, renderable );
      ecs_coordinator_->addComponent<SpriteComponent>( entity, SpriteComponent( window_texture ) );
    }
  }

  void Platformer::draw_nano_model() noexcept {

    BenchmarkTimer timer( "draw_nano_model()" );

    auto model = assets_manager_->storeAsset<Model>(
        "nanosuit",
        assets_manager_->createBuilder<Model>( assets_manager_ )
            .setModelPath( FileSystem::getPath( "assets/models/nanosuit/nanosuit.obj" ) )
            .build() );

    auto model_mesh = model->meshes;

    nano_model_entity_ = ecs_coordinator_->createEntity();
    ecs_coordinator_->addComponent<Relationship>( nano_model_entity_, Relationship() );

    for ( const auto &i : model_mesh ) {

      auto entity = ecs_coordinator_->createEntity();

      ecs_coordinator_->addComponent<Relationship>( entity, Relationship() );
      ecs_coordinator_->attachTo( nano_model_entity_, entity );

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

      ecs_coordinator_->addComponent<Transform>( entity, transform );
      ecs_coordinator_->addComponent<Renderable>( entity, renderable );
      ecs_coordinator_->addComponent<MeshComponent>( entity, mesh );
    }
  }

  void Platformer::draw_text_font() noexcept {

    BenchmarkTimer timer( "draw_font()" );

    auto sfmono_font = assets_manager_->loadAsset<Font>(
        "sfmono_font", FileSystem::getPath( "assets/fonts/LiberationMono-Regular.ttf" ) );

    auto entity = ecs_coordinator_->createEntity();

    FontComponent font;
    font.font = sfmono_font;
    font.text = "This is testing font";

    Renderable renderable;
    renderable.color = glm::vec3( 0.9f, 0.2f, 0.2f );

    Transform transform;
    transform.position = glm::vec3( 22.0f, 0.0f, 0.0f );
    transform.scale = glm::vec3( 1.0f );

    ecs_coordinator_->addComponent<Transform>( entity, transform );
    ecs_coordinator_->addComponent<Renderable>( entity, renderable );
    ecs_coordinator_->addComponent<FontComponent>( entity, font );
  }

  void Platformer::draw_point_lights() noexcept {

    BenchmarkTimer timer( "draw_point_lights()" );

    auto model = assets_manager_->storeAsset<Model>(
        "light_sphere",
        assets_manager_->createBuilder<Model>( assets_manager_ )
            .setModelPath( FileSystem::getPath( "assets/models/sphere/sphere.obj" ) )
            .build() );

    auto model_mesh = model->meshes;

    for ( const auto &i : model_mesh ) {

      lamp_entity_ = ecs_coordinator_->createEntity();

      Transform transform;
      transform.position = point_lights_positions_[ 4 ];
      transform.scale = glm::vec3( 1.0f );

      MeshComponent mesh;
      mesh.vertices = i.verticies;
      mesh.textures = i.textures;
      mesh.indices = i.indices;

      Renderable renderable;
      renderable.color = glm::vec3( 1.0f, 1.0f, 1.0f );
      renderable.blend = false;
      renderable.shaderSet = assets_manager_->getAsset<ShaderSet>( "lamp_shader" );

      ecs_coordinator_->addComponent<Transform>( lamp_entity_, transform );
      ecs_coordinator_->addComponent<Renderable>( lamp_entity_, renderable );
      ecs_coordinator_->addComponent<MeshComponent>( lamp_entity_, mesh );
    }
  }

  void Platformer::draw_sprites_test() noexcept {

    BenchmarkTimer timer( "draw_sprite_test()" );

    sprite_entity_ = ecs_coordinator_->createEntity();

    auto texture = assets_manager_->loadAsset<Texture2D>(
        "knight_attack", FileSystem::getPath( "assets/textures/Knight/knight_attack.png" ) );

    SpriteComponent sprite;
    sprite.texture = texture;

    Transform transform;
    transform.position = glm::vec3( 6.0f, 0.0f, 1.0f );
    transform.scale = glm::vec3( 3.0f );

    Renderable renderable;
    renderable.color = glm::vec3( 1.0f, 1.0f, 1.0f );
    renderable.blend = false;

    ecs_coordinator_->addComponent<Transform>( sprite_entity_, transform );
    ecs_coordinator_->addComponent<Renderable>( sprite_entity_, renderable );
    ecs_coordinator_->addComponent<SpriteComponent>( sprite_entity_, sprite );
  }

  void Platformer::draw_pizza_box() noexcept {

    BenchmarkTimer timer( "draw_pizza_box()" );

    auto model = assets_manager_->storeAsset<Model>(
        "piazza_box",
        assets_manager_->createBuilder<Model>( assets_manager_ )
            .setModelPath( FileSystem::getPath( "assets/models/pizza_box/pizza_box.obj" ) )
            .build() );

    auto model_mesh = model->meshes;

    nano_model_entity_ = ecs_coordinator_->createEntity();
    ecs_coordinator_->addComponent<Relationship>( nano_model_entity_, Relationship() );

    for ( const auto &i : model_mesh ) {

      auto entity = ecs_coordinator_->createEntity();

      ecs_coordinator_->addComponent<Relationship>( entity, Relationship() );
      ecs_coordinator_->attachTo( nano_model_entity_, entity );

      Transform transform;
      transform.position = glm::vec3( 22.0f, 6.3f, 18.0f );
      // transform.yRotation = 45.0f;
      transform.xRotation = -90.0f;
      transform.zRotation = -17.0f;
      transform.scale = glm::vec3( 0.044f );

      MeshComponent mesh;
      mesh.vertices = i.verticies;
      mesh.textures = i.textures;
      mesh.indices = i.indices;

      Renderable renderable;
      renderable.color = glm::vec3( 1.0f, 1.0f, 1.0f );
      renderable.blend = false;

      ecs_coordinator_->addComponent<Transform>( entity, transform );
      ecs_coordinator_->addComponent<Renderable>( entity, renderable );
      ecs_coordinator_->addComponent<MeshComponent>( entity, mesh );
    }
  }

  void Platformer::draw_micro_subscene() noexcept {

    BenchmarkTimer timer( "draw_micro_subscene()" );

    auto model = assets_manager_->storeAsset<Model>(
        "metal_container",
        assets_manager_->createBuilder<Model>( assets_manager_ )
            .setModelPath( FileSystem::getPath( "assets/models/container/container.obj" ) )
            .build() );
    auto model_mesh = model->meshes;

    subscene_entity_ = ecs_coordinator_->createEntity();
    ecs_coordinator_->addComponent<Relationship>( subscene_entity_, Relationship() );

    f32 offset = 6.0f;

    for ( size_t j = 0; j < 4; j++ ) {

      point_lights_positions_.emplace_back( ( 12.0f + ( j * offset ) ), 18.0f, 62.0f );

      for ( const auto &i : model_mesh ) {

        auto entity = ecs_coordinator_->createEntity();

        ecs_coordinator_->addComponent<Relationship>( entity, Relationship() );
        ecs_coordinator_->attachTo( subscene_entity_, entity );

        Transform transform;
        transform.position = glm::vec3( ( 12.0f + ( j * offset ) ), 0.0f, 62.0f );
        transform.xRotation = -90.0f;
        transform.scale = glm::vec3( 0.1f );

        MeshComponent mesh;
        mesh.vertices = i.verticies;
        mesh.textures = i.textures;
        mesh.indices = i.indices;

        Renderable renderable;
        renderable.color = glm::vec3( 1.0f, 1.0f, 1.0f );
        renderable.blend = false;

        ecs_coordinator_->addComponent<Transform>( entity, transform );
        ecs_coordinator_->addComponent<Renderable>( entity, renderable );
        ecs_coordinator_->addComponent<MeshComponent>( entity, mesh );
      }
    }

    spdlog::debug( "size of relationship comp: {}",
                   ecs_coordinator_->get_relationship_size( subscene_entity_ ) );

    auto f = ecs_coordinator_->getFirst( subscene_entity_ );

    while ( f != nile::ecs::null ) {

      spdlog::debug( "entity: {}", f );
      f = ecs_coordinator_->getNext( f );
    }

    point_lights_positions_.emplace_back( 32.0f, 16.0f, 19.0f );

    spdlog::debug( "num of points: {}", point_lights_positions_.size() );

    auto light_model = assets_manager_->storeAsset<Model>(
        "light_sphere",
        assets_manager_->createBuilder<Model>( assets_manager_ )
            .setModelPath( FileSystem::getPath( "assets/models/sphere/sphere.obj" ) )
            .build() );

    auto light_model_mesh = light_model->meshes;

    for ( int j = 0; j < 4; j++ ) {
      for ( const auto &i : light_model_mesh ) {
        lamp_entity_ = ecs_coordinator_->createEntity();

        Transform transform;
        transform.position = point_lights_positions_[ j ];
        transform.scale = glm::vec3( 1.0f );

        MeshComponent mesh;
        mesh.vertices = i.verticies;
        mesh.textures = i.textures;
        mesh.indices = i.indices;

        Renderable renderable;
        renderable.color = glm::vec3( 1.0f, 1.0f, 1.0f );
        renderable.blend = false;
        renderable.shaderSet = assets_manager_->getAsset<ShaderSet>( "lamp_shader" );

        ecs_coordinator_->addComponent<Transform>( lamp_entity_, transform );
        ecs_coordinator_->addComponent<Renderable>( lamp_entity_, renderable );
        ecs_coordinator_->addComponent<MeshComponent>( lamp_entity_, mesh );
      }
    }
  }

}    // namespace platformer
