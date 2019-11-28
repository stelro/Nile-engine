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
#include "Nile/debug/benchmark_timer.hh"
#include "Nile/ecs/components/camera_component.hh"
#include "Nile/ecs/components/font_component.hh"
#include "Nile/ecs/components/mesh_component.hh"
#include "Nile/ecs/components/primitive.hh"
#include "Nile/ecs/components/renderable.hh"
#include "Nile/ecs/components/sprite.hh"
#include "Nile/ecs/components/transform.hh"
#include "Nile/ecs/ecs_coordinator.hh"
#include "Nile/log/log.hh"
#include "Nile/log/stream_logger.hh"
#include "Nile/renderer/base_renderer.hh"
#include "Nile/renderer/font_rendering_system.hh"
#include "Nile/renderer/opengl_framebuffer.hh"
#include "Nile/renderer/opengl_renderer.hh"
#include "Nile/renderer/render_primitive_system.hh"
#include "Nile/renderer/rendering_system.hh"
#include "Nile/renderer/sprite_rendering_system.hh"
#include "Nile/renderer/texture2d.hh"

#include <GL/glew.h>
#include <SDL2/SDL.h>


namespace nile::X11 {

  class GameHostX11::Impl {
  private:
    // Just to keep things orginized
    void registerEcs() noexcept;

    std::unique_ptr<OpenglFramebuffer> m_framebuffer;

    std::shared_ptr<RenderingSystem> renderingSystem;
    std::shared_ptr<SpriteRenderingSystem> spriteRenderingSystem;
    std::shared_ptr<RenderPrimitiveSystem> renderPrimitiveSystem;
    std::shared_ptr<FontRenderingSystem> fontRenderingSystem;


    std::shared_ptr<ShaderSet> m_fbScreenShader;

    Timer m_uptime;

    ProgramMode m_programMode;

  public:
    Impl( const std::shared_ptr<Settings> &settings ) noexcept;
    ~Impl() noexcept;

    void run( Game &game ) noexcept;

    std::shared_ptr<Settings> settings;
    std::shared_ptr<BaseRenderer> renderer;
    std::shared_ptr<InputManager> inputManager;
    std::shared_ptr<AssetManager> assetManager;
    std::shared_ptr<Coordinator> ecsCoordinator;
  };

  GameHostX11::Impl::Impl( const std::shared_ptr<Settings> &settings ) noexcept
      : settings( settings ) {

    // @IMPORTANT FIX(stel): correct the orrder of creation and initialization
    // of systems and subsystems ESPECIALLY RENDERING SYSTEM

    m_uptime.start();

    // Connect log output to std stream logger ( console / terminal )
    log::on_message.connect(
        []( const char *msg, LogType type ) { StreamLogger::printToStream( msg, type ); } );

    inputManager = std::make_shared<InputManager>( settings );
    ( inputManager ) ? log::notice( "InputManager have been created!\n" )
                     : log::fatal( "Engine has failed to create InputManager!\n" );

    renderer = std::make_shared<OpenGLRenderer>( settings );
    renderer->init();

    ( renderer ) ? log::notice( "OpenGL Renderer have been created and initialized!\n" )
                 : log::fatal( "Engine has failed to create OpenGL Renderer!\n" );

    m_framebuffer = std::make_unique<OpenglFramebuffer>( settings );
    m_framebuffer->prepareQuad();
    ( m_framebuffer ) ? log::print( "\tOpenGL Frambuffer have been created and initialized!\n" )
                      : log::fatal( "Engine has failed to create OpenGL Framebuffer!\n" );


    assetManager = std::make_shared<AssetManager>();
    ( assetManager ) ? log::notice( "AssetManager have been created!\n" )
                     : log::fatal( "Engine has failed to create AssetManager!\n" );

    // Create and initialize Entity Component System coordinator
    ecsCoordinator = std::make_shared<Coordinator>();
    ecsCoordinator->init();
    ( ecsCoordinator ) ? log::notice( "ECS Coordinator have been created!\n" )
                       : log::fatal( "Engine has failed to create ECS Coordinator!\n" );


    m_programMode = settings->getProgramMode();

    auto spriteShader = assetManager->createBuilder<ShaderSet>()
                            .setVertexPath( "../assets/shaders/sprite_vertex.glsl" )
                            .setFragmentPath( "../assets/shaders/sprite_fragment.glsl" )
                            .build();

    assetManager->storeAsset<ShaderSet>( "sprite_shader", spriteShader );

    auto fontShader = assetManager->createBuilder<ShaderSet>()
                          .setVertexPath( "../assets/shaders/font_vertex.glsl" )
                          .setFragmentPath( "../assets/shaders/font_fragment.glsl" )
                          .build();

    assetManager->storeAsset<ShaderSet>( "font_shader", fontShader );

    auto lineShader = assetManager->createBuilder<ShaderSet>()
                          .setVertexPath( "../assets/shaders/line_vertex.glsl" )
                          .setFragmentPath( "../assets/shaders/line_fragment.glsl" )
                          .build();

    assetManager->storeAsset<ShaderSet>( "line_shader", lineShader );

    auto modelShader = assetManager->createBuilder<ShaderSet>()
                           .setVertexPath( "../assets/shaders/model_vertex.glsl" )
                           .setFragmentPath( "../assets/shaders/model_fragment.glsl" )
                           .build();


    assetManager->storeAsset<ShaderSet>( "model_shader", modelShader );


    // main window framebuffer shader
    m_fbScreenShader = assetManager->storeAsset<ShaderSet>(
        "fb_screen_shader",
        assetManager->createBuilder<ShaderSet>()
            .setVertexPath( FileSystem::getBuildDir() + "/resources/shaders/screen_fb_vertex.glsl" )
            .setFragmentPath( FileSystem::getBuildDir() +
                              "/resources/shaders/screen_fb_fragment.glsl" )
            .build() );


    this->registerEcs();
  }

  void GameHostX11::Impl::registerEcs() noexcept {


    // some default components to begin with
    // Register components to the entity-component-system
    ecsCoordinator->registerComponent<Transform>();
    ecsCoordinator->registerComponent<Renderable>();
    ecsCoordinator->registerComponent<SpriteComponent>();
    ecsCoordinator->registerComponent<CameraComponent>();
    ecsCoordinator->registerComponent<Primitive>();
    ecsCoordinator->registerComponent<MeshComponent>();
    ecsCoordinator->registerComponent<FontComponent>();

    // Output some logs to know which components has been registered by the engine
    log::notice( "Registered ECS components by the engine: \n"
                 "\t Transform\n"
                 "\t Renderable\n"
                 "\t SpriteComponent\n"
                 "\t CameraComponent\n"
                 "\t Primitive\n"
                 "\t MeshComponent\n"
                 "\t FontComponent\n" );

    renderingSystem = ecsCoordinator->registerSystem<RenderingSystem>(
        ecsCoordinator, assetManager->getAsset<ShaderSet>( "model_shader" ) );

    spriteRenderingSystem = ecsCoordinator->registerSystem<SpriteRenderingSystem>(
        ecsCoordinator, assetManager->getAsset<ShaderSet>( "sprite_shader" ) );

    renderPrimitiveSystem = ecsCoordinator->registerSystem<RenderPrimitiveSystem>(
        ecsCoordinator, assetManager->getAsset<ShaderSet>( "line_shader" ) );

    fontRenderingSystem = ecsCoordinator->registerSystem<FontRenderingSystem>(
        ecsCoordinator, settings, assetManager->getAsset<ShaderSet>( "font_shader" ) );

    auto cameraSystem = ecsCoordinator->registerSystem<CameraSystem>( ecsCoordinator, settings );

    log::notice( "Registered ECS systems by the engine: \n"
                 "\t SpriteRenderingSystem\n"
                 "\t RenderPrimitiveSystem\n"
                 "\t RenderingSystem\n"
                 "\t CameraSystem\n" );

    Signature signature;
    signature.set( ecsCoordinator->getComponentType<Transform>() );
    signature.set( ecsCoordinator->getComponentType<Renderable>() );
    signature.set( ecsCoordinator->getComponentType<SpriteComponent>() );
    ecsCoordinator->setSystemSignature<SpriteRenderingSystem>( signature );

    Signature cameraSignature;
    cameraSignature.set( ecsCoordinator->getComponentType<Transform>() );
    cameraSignature.set( ecsCoordinator->getComponentType<CameraComponent>() );
    ecsCoordinator->setSystemSignature<CameraSystem>( cameraSignature );

    Signature primSignature;
    primSignature.set( ecsCoordinator->getComponentType<Transform>() );
    primSignature.set( ecsCoordinator->getComponentType<Renderable>() );
    primSignature.set( ecsCoordinator->getComponentType<Primitive>() );
    ecsCoordinator->setSystemSignature<RenderPrimitiveSystem>( primSignature );

    Signature renderingSignature;
    renderingSignature.set( ecsCoordinator->getComponentType<Transform>() );
    renderingSignature.set( ecsCoordinator->getComponentType<Renderable>() );
    renderingSignature.set( ecsCoordinator->getComponentType<MeshComponent>() );
    ecsCoordinator->setSystemSignature<RenderingSystem>( renderingSignature );

    Signature fontSignature;
    fontSignature.set( ecsCoordinator->getComponentType<Transform>() );
    fontSignature.set( ecsCoordinator->getComponentType<Renderable>() );
    fontSignature.set( ecsCoordinator->getComponentType<FontComponent>() );
    ecsCoordinator->setSystemSignature<FontRenderingSystem>( fontSignature );
  }

  GameHostX11::Impl::~Impl() noexcept {}

  void GameHostX11::Impl::run( Game &game ) noexcept {

    game.initialize();
    ecsCoordinator->createSystems();
    f64 lastStep = SDL_GetTicks();

    // draw wireframe
    // glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );

    while ( !inputManager->shouldClose() ) {

      f64 currentStep = SDL_GetTicks();
      f64 delta = currentStep - lastStep;    // elapsed time

      inputManager->update( delta );

      // @fix(stel) : move all of these framebuffer related stuff
      // to the framebuffer class
      renderer->submitFrame();
      m_framebuffer->bind();
      glEnable( GL_DEPTH_TEST );
      glClearColor( 0.1f, 0.1f, 0.1f, 1.0f );
      glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

      ecsCoordinator->update( delta );
      ecsCoordinator->render( delta );

      game.update( delta );

      m_framebuffer->unbind();
      glDisable( GL_DEPTH_TEST );
      glClearColor( 1.0f, 1.0f, 1.0f, 1.0f );
      glClear( GL_COLOR_BUFFER_BIT );

      m_fbScreenShader->use();
      m_framebuffer->submitFrame();
      renderer->endFrame();

      m_programMode = settings->getProgramMode();

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

  [[nodiscard]] std::shared_ptr<Settings> GameHostX11::getSettings() const noexcept {
    return this->m_settings;
  }

  [[nodiscard]] std::shared_ptr<BaseRenderer> GameHostX11::getRenderer() const noexcept {
    return this->impl->renderer;
  }

  [[nodiscard]] std::shared_ptr<InputManager> GameHostX11::getInputManager() const noexcept {
    return this->impl->inputManager;
  }

  [[nodiscard]] std::shared_ptr<AssetManager> GameHostX11::getAssetManager() const noexcept {
    return this->impl->assetManager;
  }

  [[nodiscard]] std::shared_ptr<Coordinator> GameHostX11::getEcsCoordinator() const noexcept {
    return this->impl->ecsCoordinator;
  }

}    // namespace nile::X11
