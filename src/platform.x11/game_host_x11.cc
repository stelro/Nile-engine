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
#include "Nile/core/input_manager.hh"
#include "Nile/core/settings.hh"
#include "Nile/core/timer.hh"
#include "Nile/ecs/components/camera_component.hh"
#include "Nile/ecs/components/mesh_component.hh"
#include "Nile/ecs/components/primitive.hh"
#include "Nile/ecs/components/renderable.hh"
#include "Nile/ecs/components/sprite.hh"
#include "Nile/ecs/components/transform.hh"
#include "Nile/ecs/ecs_coordinator.hh"
#include "Nile/editor/editor.hh"
#include "Nile/log/log.hh"
#include "Nile/renderer/base_renderer.hh"
#include "Nile/renderer/opengl_renderer.hh"
#include "Nile/renderer/render_primitive_system.hh"
#include "Nile/renderer/rendering_system.hh"
#include "Nile/renderer/sprite_rendering_system.hh"
#include "Nile/renderer/texture2d.hh"

#include <GL/glew.h>
#include <SDL2/SDL.h>


namespace nile::X11 {

  using namespace editor;

  class GameHostX11::Impl {
  private:
    std::shared_ptr<RenderingSystem> renderingSystem;
    std::shared_ptr<SpriteRenderingSystem> spriteRenderingSystem;
    std::shared_ptr<RenderPrimitiveSystem> renderPrimitiveSystem;

    Timer m_uptime;

    static const u32 NUM_FPS_SAMPLES = 256;
    f32 m_samples[ NUM_FPS_SAMPLES ];
    u32 m_currentFrame = 0;

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

    std::unique_ptr<Editor> editor;
  };

  GameHostX11::Impl::Impl( const std::shared_ptr<Settings> &settings ) noexcept
      : settings( settings ) {

    m_uptime.start();

    renderer = std::make_shared<OpenGLRenderer>( settings );
    renderer->init();

    m_programMode = settings->getProgramMode();

    // Editor stuff
    editor = std::make_unique<Editor>( renderer->getWindow(), renderer->getContext() );

    inputManager = std::make_shared<InputManager>();
    assetManager = std::make_shared<AssetManager>();

    // Register TextureLoader to the AssetManager
    assetManager->registerLoader<Texture2D, TextureLoader>( true );

    auto spriteShader = assetManager->createBuilder<ShaderSet>()
                            .setVertexPath( "../assets/shaders/sprite_vertex.glsl" )
                            .setFragmentPath( "../assets/shaders/sprite_fragment.glsl" )
                            .build();

    assetManager->storeAsset<ShaderSet>( "sprite_shader", spriteShader );

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


    ecsCoordinator = std::make_shared<Coordinator>();
    ecsCoordinator->init();

    // some default components to begin with
    // Register components to the entity-component-system
    ecsCoordinator->registerComponent<Transform>();
    ecsCoordinator->registerComponent<Renderable>();
    ecsCoordinator->registerComponent<SpriteComponent>();
    ecsCoordinator->registerComponent<CameraComponent>();
    ecsCoordinator->registerComponent<Primitive>();
    ecsCoordinator->registerComponent<MeshComponent>();


    spriteRenderingSystem = ecsCoordinator->registerSystem<SpriteRenderingSystem>(
        ecsCoordinator, assetManager->getAsset<ShaderSet>( "sprite_shader" ) );

    renderPrimitiveSystem = ecsCoordinator->registerSystem<RenderPrimitiveSystem>(
        ecsCoordinator, assetManager->getAsset<ShaderSet>( "line_shader" ) );

    renderingSystem = ecsCoordinator->registerSystem<RenderingSystem>(
        ecsCoordinator, assetManager->getAsset<ShaderSet>( "model_shader" ) );

    auto cameraSystem = ecsCoordinator->registerSystem<CameraSystem>( ecsCoordinator, settings );

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
  }

  GameHostX11::Impl::~Impl() noexcept {}

  void GameHostX11::Impl::run( Game &game ) noexcept {

    game.initialize();
    ecsCoordinator->createSystems();
    f64 lastStep = SDL_GetTicks();


    while ( !inputManager->shouldClose() ) {

      // TODO(stel): I should fix that ( calculate time between frames )
      f64 currentStep = SDL_GetTicks();
      f64 delta = currentStep - lastStep;    // elapsed time
      f32 avgFps = 0.0f;

      if ( m_programMode == ProgramMode::EDITOR_MODE ) {
        m_samples[ m_currentFrame % NUM_FPS_SAMPLES ] = delta;

        for ( u32 i = 0; i < NUM_FPS_SAMPLES; i++ )
          avgFps += m_samples[ i ];

        if ( m_currentFrame >= NUM_FPS_SAMPLES )
          m_currentFrame = 0;

        avgFps /= NUM_FPS_SAMPLES;
      }

      inputManager->update( delta );

      renderer->submitFrame();

      ecsCoordinator->update( delta );
      ecsCoordinator->render( delta );

      if ( m_programMode == ProgramMode::EDITOR_MODE ) {
        editor->render( delta );
        editor->update( delta );
      }

      if ( inputManager->isKeyPressed( SDLK_k ) ) {
        log::console( "pressed\n" );
      }

      game.update( delta );

      renderer->endFrame();

      editor->setFps( avgFps );
      editor->setEntities( ecsCoordinator->getEntitiesCount() );
      editor->setComponents( ecsCoordinator->getComponentsCount() );
      editor->setEcsSystems( ecsCoordinator->getSystemsCount() );
      editor->setUptime( m_uptime.getTicks() );
      editor->setLoadersCount( assetManager->getLoadersCount() );

      if ( inputManager->isKeyPressed( SDLK_F1 ) ) {
        if ( m_programMode != ProgramMode::EDITOR_MODE )
          settings->setProgramMode( ProgramMode::EDITOR_MODE );
        else
          settings->setProgramMode( ProgramMode::GAME_MODE );
      }

      m_programMode = settings->getProgramMode();

      lastStep = currentStep;
      m_currentFrame++;
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
