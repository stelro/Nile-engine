/* ================================================================================
$File: game_host_x11.cc
$Date: $
$Revision: $
$Creator: Rostislav Orestis Stelmach
$Notice: $
================================================================================ */

#include "Nile/platform/x11/game_host_x11.hh"
#include "Nile/application/game.hh"
#include "Nile/asset/asset_manager.hh"
#include "Nile/asset/builder/shaderset_builder.hh"
#include "Nile/asset/subsystem/texture_loader.hh"
#include "Nile/core/camera_system.hh"
#include "Nile/core/file_system.hh"
#include "Nile/core/input_manager.hh"
#include "Nile/core/settings.hh"
#include "Nile/core/timer.hh"
#include "Nile/core/transform_system.hh"
#include "Nile/debug/benchmark_timer.hh"
#include "Nile/ecs/components/camera_component.hh"
#include "Nile/ecs/components/font_component.hh"
#include "Nile/ecs/components/mesh_component.hh"
#include "Nile/ecs/components/primitive.hh"
#include "Nile/ecs/components/relationship.hh"
#include "Nile/ecs/components/renderable.hh"
#include "Nile/ecs/components/sprite.hh"
#include "Nile/ecs/components/transform.hh"
#include "Nile/ecs/ecs_coordinator.hh"
#include "Nile/experimental/asset/asset_manager_helper.hh"
#include "Nile/renderer/base_renderer.hh"
#include "Nile/renderer/font_rendering_system.hh"
#include "Nile/renderer/opengl_framebuffer.hh"
#include "Nile/renderer/opengl_renderer.hh"
#include "Nile/renderer/render_primitive_system.hh"
#include "Nile/renderer/rendering_system.hh"
#include "Nile/renderer/sprite_rendering_system.hh"
#include "Nile/renderer/texture2d.hh"
#include "spdlog/common.h"

#include <GL/glew.h>
#include <SDL2/SDL.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/spdlog.h>

#include <thread>

namespace nile::X11 {

  class GameHostX11::Impl {
  private:
    // Just to keep things orginized
    void initializ_ecs_subsystems() noexcept;

    std::unique_ptr<OpenglFramebuffer> frame_buffer_;

    std::shared_ptr<RenderingSystem> rendering_system_;
    std::shared_ptr<SpriteRenderingSystem> sprite_rendering_system_;
    std::shared_ptr<RenderPrimitiveSystem> rendering_primitive_system_;
    std::shared_ptr<FontRenderingSystem> font_rendering_system_;
    std::shared_ptr<TransformSystem> transform_system_;

    std::shared_ptr<AssetManagerHelper> assets_manager_helper_;
    std::shared_ptr<ShaderSet> framebuffer_screen_shader_;

    Timer uptime_;
    ProgramMode program_mode_;

  public:
    Impl( const std::shared_ptr<Settings> &settings ) noexcept;
    ~Impl() noexcept;

    void run( Game &game ) noexcept;

    std::shared_ptr<Settings> settings;
    std::shared_ptr<BaseRenderer> renderer;
    std::shared_ptr<InputManager> input_manager;
    std::shared_ptr<AssetManager> assets_manager;
    std::shared_ptr<Coordinator> ecs_coordinator;
  };

  GameHostX11::Impl::Impl( const std::shared_ptr<Settings> &settings ) noexcept
      : settings( settings ) {

    // @IMPORTANT FIX(stel): correct the orrder of creation and initialization
    // of systems and subsystems ESPECIALLY RENDERING SYSTEM

    // spdlog / logger settings
    spdlog::set_level( spdlog::level::debug );

    uptime_.start();

    input_manager = std::make_shared<InputManager>( settings );
    ( input_manager ) ? spdlog::info( "input_manager has been created!" )
                      : spdlog::critical( "Engine has failed to create input_manager!" );

    renderer = std::make_shared<OpenGLRenderer>( settings );
    renderer->init();

    ( renderer ) ? spdlog::info( "OpenGL Renderer has been created and initialized!" )
                 : spdlog::critical( "Engine has failed to create OpenGL Renderer!" );

    frame_buffer_ = std::make_unique<OpenglFramebuffer>( settings );
    frame_buffer_->prepareQuad();
    ( frame_buffer_ ) ? spdlog::info( "OpenGL Frambuffer has been created and initialized!" )
                      : spdlog::critical( "Engine has failed to create OpenGL Framebuffer!" );


    assets_manager = std::make_shared<AssetManager>();
    ( assets_manager ) ? spdlog::info( "assets_manager has been created!" )
                       : spdlog::critical( "Engine has failed to create assets_manager!" );

    // Create and initialize Entity Component System coordinator
    ecs_coordinator = std::make_shared<Coordinator>();
    ecs_coordinator->init();
    ( ecs_coordinator ) ? spdlog::info( "ECS Coordinator has been created!" )
                        : spdlog::critical( "Engine has failed to create ECS Coordinator!" );

    assets_manager_helper_ = std::make_shared<AssetManagerHelper>( assets_manager );
    program_mode_ = settings->getProgramMode();

    auto sprite_shader = assets_manager->createBuilder<ShaderSet>()
                             .setVertexPath( "../assets/shaders/sprite_vertex.glsl" )
                             .setFragmentPath( "../assets/shaders/sprite_fragment.glsl" )
                             .build();

    assets_manager->storeAsset<ShaderSet>( "sprite_shader", sprite_shader );

    auto font_shader = assets_manager->createBuilder<ShaderSet>()
                           .setVertexPath( "../assets/shaders/font_vertex.glsl" )
                           .setFragmentPath( "../assets/shaders/font_fragment.glsl" )
                           .build();

    assets_manager->storeAsset<ShaderSet>( "font_shader", font_shader );

    auto line_shader = assets_manager->createBuilder<ShaderSet>()
                           .setVertexPath( "../assets/shaders/line_vertex.glsl" )
                           .setFragmentPath( "../assets/shaders/line_fragment.glsl" )
                           .build();

    assets_manager->storeAsset<ShaderSet>( "line_shader", line_shader );

    auto model_shader = assets_manager->createBuilder<ShaderSet>()
                            .setVertexPath( "../assets/shaders/model.vert.glsl" )
                            .setFragmentPath( "../assets/shaders/model.frag.glsl" )
                            .build();

    assets_manager->storeAsset<ShaderSet>( "model_shader", model_shader );

    // main window framebuffer shader
    framebuffer_screen_shader_ = assets_manager->storeAsset<ShaderSet>(
        "fb_screen_shader", assets_manager->createBuilder<ShaderSet>()
                                .setVertexPath( FileSystem::getBinaryDir() +
                                                "/resources/shaders/screen_fb_vertex.glsl" )
                                .setFragmentPath( FileSystem::getBinaryDir() +
                                                  "/resources/shaders/screen_fb_fragment.glsl" )
                                .build() );

    this->initializ_ecs_subsystems();
  }

  void GameHostX11::Impl::initializ_ecs_subsystems() noexcept {

    // some default components to begin with
    // Register components to the entity-component-system
    ecs_coordinator->registerComponent<Transform>();
    ecs_coordinator->registerComponent<Renderable>();
    ecs_coordinator->registerComponent<SpriteComponent>();
    ecs_coordinator->registerComponent<CameraComponent>();
    ecs_coordinator->registerComponent<Primitive>();
    ecs_coordinator->registerComponent<MeshComponent>();
    ecs_coordinator->registerComponent<FontComponent>();
    ecs_coordinator->registerComponent<Relationship>();

    // Output some logs to know which components has been registered by the engine
    spdlog::info( "Registered ECS components by the engine: "
                  "[ Transform, Renderable, SpriteComponent, CameraComponent, Primitive, "
                  "MeshComponent, FontComponent, Renletionship ]" );

    rendering_system_ = ecs_coordinator->registerSystem<RenderingSystem>(
        ecs_coordinator, assets_manager->getAsset<ShaderSet>( "model_shader" ) );

    sprite_rendering_system_ = ecs_coordinator->registerSystem<SpriteRenderingSystem>(
        ecs_coordinator, assets_manager->getAsset<ShaderSet>( "model_shader" ) );

    rendering_primitive_system_ = ecs_coordinator->registerSystem<RenderPrimitiveSystem>(
        ecs_coordinator, assets_manager->getAsset<ShaderSet>( "line_shader" ) );

    font_rendering_system_ = ecs_coordinator->registerSystem<FontRenderingSystem>(
        ecs_coordinator, settings, assets_manager->getAsset<ShaderSet>( "font_shader" ) );

    transform_system_ = ecs_coordinator->registerSystem<TransformSystem>( ecs_coordinator );

    auto cameraSystem = ecs_coordinator->registerSystem<CameraSystem>( ecs_coordinator, settings );

    spdlog::info(
        "Registered ECS systems by the engine: "
        "[ SpriteRenderingSystem, RenderingPrimitiveSystem, RenderingSystem, CameraSystem, TransformSystem ]" );

    Signature sprite_signature;
    sprite_signature.set( ecs_coordinator->getComponentType<Transform>() );
    sprite_signature.set( ecs_coordinator->getComponentType<Renderable>() );
    sprite_signature.set( ecs_coordinator->getComponentType<SpriteComponent>() );
    ecs_coordinator->setSystemSignature<SpriteRenderingSystem>( sprite_signature );

    Signature camera_signature;
    camera_signature.set( ecs_coordinator->getComponentType<Transform>() );
    camera_signature.set( ecs_coordinator->getComponentType<CameraComponent>() );
    ecs_coordinator->setSystemSignature<CameraSystem>( camera_signature );

    Signature primitive_signature;
    primitive_signature.set( ecs_coordinator->getComponentType<Transform>() );
    primitive_signature.set( ecs_coordinator->getComponentType<Renderable>() );
    primitive_signature.set( ecs_coordinator->getComponentType<Primitive>() );
    ecs_coordinator->setSystemSignature<RenderPrimitiveSystem>( primitive_signature );

    Signature rendering_signature;
    rendering_signature.set( ecs_coordinator->getComponentType<Transform>() );
    rendering_signature.set( ecs_coordinator->getComponentType<Renderable>() );
    rendering_signature.set( ecs_coordinator->getComponentType<MeshComponent>() );
    ecs_coordinator->setSystemSignature<RenderingSystem>( rendering_signature );

    Signature font_signature;
    font_signature.set( ecs_coordinator->getComponentType<Transform>() );
    font_signature.set( ecs_coordinator->getComponentType<Renderable>() );
    font_signature.set( ecs_coordinator->getComponentType<FontComponent>() );
    ecs_coordinator->setSystemSignature<FontRenderingSystem>( font_signature );

    Signature transform_signature;
    transform_signature.set( ecs_coordinator->getComponentType<Transform>() );
    transform_signature.set( ecs_coordinator->getComponentType<Relationship>() );
    ecs_coordinator->setSystemSignature<TransformSystem>( transform_signature );

  }

  GameHostX11::Impl::~Impl() noexcept {}

  void GameHostX11::Impl::run( Game &game ) noexcept {

    game.initialize();
    ecs_coordinator->createSystems();
    f64 lastStep = SDL_GetTicks();

    // draw wireframe
    // glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );

    // u32 frame = 0;

    while ( !input_manager->shouldClose() ) {

      //  log::print("[%d]\n", frame++);

      f64 currentStep = SDL_GetTicks();
      f64 delta = currentStep - lastStep;    // elapsed time

      input_manager->update( delta );

      // @fix(stel) : move all of these framebuffer related stuff
      // to the framebuffer class
      renderer->submitFrame();
      frame_buffer_->bind();
      glEnable( GL_DEPTH_TEST );
      glClearColor( 0.1f, 0.1f, 0.1f, 1.0f );
      //     glClearColor( 0.635f, 0.851f, 0.808f, 1.0f );
      glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

      ecs_coordinator->update( delta );
      ecs_coordinator->render( delta );

      game.update( delta );

      frame_buffer_->unbind();
      glDisable( GL_DEPTH_TEST );
      glClearColor( 1.0f, 1.0f, 1.0f, 1.0f );
      glClear( GL_COLOR_BUFFER_BIT );

      framebuffer_screen_shader_->use();
      frame_buffer_->submitFrame();
      renderer->endFrame();

      program_mode_ = settings->getProgramMode();

      lastStep = currentStep;
    }
  }

  GameHostX11::GameHostX11( const std::shared_ptr<Settings> &settings ) noexcept
      : GameHost( settings )
      , impl( std::make_unique<Impl>( settings ) ) {}

  GameHostX11::~GameHostX11() noexcept {
    // Empty Destructor
  }

  void GameHostX11::run( Game &game ) noexcept {
    impl->run( game );
  }

  [[nodiscard]] std::shared_ptr<Settings> GameHostX11::get_settings() const noexcept {
    return this->settings_;
  }

  [[nodiscard]] std::shared_ptr<BaseRenderer> GameHostX11::get_renderer() const noexcept {
    return this->impl->renderer;
  }

  [[nodiscard]] std::shared_ptr<InputManager> GameHostX11::get_input_manager() const noexcept {
    return this->impl->input_manager;
  }

  [[nodiscard]] std::shared_ptr<AssetManager> GameHostX11::get_asset_manager() const noexcept {
    return this->impl->assets_manager;
  }

  [[nodiscard]] std::shared_ptr<Coordinator> GameHostX11::get_ecs_coordinator() const noexcept {
    return this->impl->ecs_coordinator;
  }

}    // namespace nile::X11
