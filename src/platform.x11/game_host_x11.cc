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
#include "Nile/core/input_manager.hh"
#include "Nile/ecs/components/renderable.hh"
#include "Nile/ecs/components/sprite.hh"
#include "Nile/ecs/components/transform.hh"
#include "Nile/ecs/ecs_coordinator.hh"
#include "Nile/renderer/base_renderer.hh"
#include "Nile/renderer/opengl_renderer.hh"
#include "Nile/renderer/rendering_system.hh"
#include "Nile/renderer/sprite_rendering_system.hh"

namespace nile::X11 {

  class GameHostX11::Impl {
  private:
    std::shared_ptr<RenderingSystem> renderingSystem;
    std::shared_ptr<SpriteRenderingSystem> spriteRenderingSystem;

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

    renderer = std::make_shared<OpenGLRenderer>( settings );
    renderer->init();

    inputManager = std::make_shared<InputManager>();
    assetManager = std::make_shared<AssetManager>();

    auto spriteShader = assetManager->createBuilder<ShaderSet>()
                            .setVertexPath( "../assets/shaders/sprite_vertex.glsl" )
                            .setFragmentPath( "../assets/shaders/sprite_fragment.glsl" )
                            .build();

    assetManager->storeAsset<ShaderSet>( "sprite_shader", spriteShader );


    ecsCoordinator = std::make_shared<Coordinator>();
    ecsCoordinator->init();

    // some default components to begin with
    // Register components to the entity-component-system
    ecsCoordinator->registerComponent<Transform>();
    ecsCoordinator->registerComponent<Renderable>();
    ecsCoordinator->registerComponent<SpriteComponent>();

    // register systems to the entity-component-system
    renderingSystem = ecsCoordinator->registerSystem<RenderingSystem>();
    renderingSystem->init( ecsCoordinator );


    spriteRenderingSystem = ecsCoordinator->registerSystem<SpriteRenderingSystem>(
        ecsCoordinator, assetManager->getAsset<ShaderSet>( "sprite_shader" ) );
    spriteRenderingSystem->init();


    Signature signature;
    signature.set( ecsCoordinator->getComponentType<Transform>() );
    signature.set( ecsCoordinator->getComponentType<Renderable>() );
    ecsCoordinator->setSystemSignature<RenderingSystem>( signature );

    signature.set( ecsCoordinator->getComponentType<SpriteComponent>() );
    ecsCoordinator->setSystemSignature<SpriteRenderingSystem>( signature );
  }

  GameHostX11::Impl::~Impl() noexcept {}

  void GameHostX11::Impl::run( Game &game ) noexcept {
    game.initialize();

    while ( !inputManager->shouldClose() ) {

      // TODO(stel): I should fix that ( calculate time between frames )
      u32 delta = 0.0f;

      inputManager->update( delta );

      renderer->submitFrame();

      // TODO(Stel): this should be fixed in the ecs coordinator.
      // the cooridinator should have it's own update function
      // where will be call every others systems update methods
     // renderingSystem->update( delta );
      spriteRenderingSystem->update(delta);
      spriteRenderingSystem->render(delta);
 //     game.update( delta );
      // game.draw( delta );

      renderer->endFrame();
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
