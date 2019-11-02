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
#include "Nile/core/camera_system.hh"
#include "Nile/core/input_manager.hh"
#include "Nile/ecs/components/camera_component.hh"
#include "Nile/ecs/components/mesh_component.hh"
#include "Nile/ecs/components/primitive.hh"
#include "Nile/ecs/components/renderable.hh"
#include "Nile/ecs/components/sprite.hh"
#include "Nile/ecs/components/transform.hh"
#include "Nile/ecs/ecs_coordinator.hh"
#include "Nile/editor/editor.hh"
#include "Nile/renderer/base_renderer.hh"
#include "Nile/renderer/opengl_renderer.hh"
#include "Nile/renderer/render_primitive_system.hh"
#include "Nile/renderer/rendering_system.hh"
#include "Nile/renderer/sprite_rendering_system.hh"

#include <GL/glew.h>
#include <SDL2/SDL.h>


namespace nile::X11 {

  using namespace editor;

  class GameHostX11::Impl {
  private:
    std::shared_ptr<RenderingSystem> renderingSystem;
    std::shared_ptr<SpriteRenderingSystem> spriteRenderingSystem;
    std::shared_ptr<RenderPrimitiveSystem> renderPrimitiveSystem;

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

    renderer = std::make_shared<OpenGLRenderer>( settings );
    renderer->init();

    // Editor stuff
    editor = std::make_unique<Editor>( renderer->getWindow(), renderer->getContext() );

    inputManager = std::make_shared<InputManager>();
    assetManager = std::make_shared<AssetManager>();

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

    log::print( "asset container size: %d\n", assetManager->getContainerSize() );

    while ( !inputManager->shouldClose() ) {


      // TODO(stel): I should fix that ( calculate time between frames )
      f64 currentStep = SDL_GetTicks();
      f64 delta = currentStep - lastStep;    // elapsed time

      inputManager->update( delta );

      renderer->submitFrame();

      ecsCoordinator->update( delta );
      ecsCoordinator->render( delta );

      editor->render( delta );
      editor->update( delta );

      game.update( delta );

      renderer->endFrame();

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
