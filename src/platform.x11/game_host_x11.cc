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
#include "Nile/core/input_manager.hh"
#include "Nile/renderer/base_renderer.hh"
#include "Nile/renderer/opengl_renderer.hh"

namespace nile::X11 {

  class GameHostX11::Impl {
  private:
  public:
    Impl( const std::shared_ptr<Settings> &settings ) noexcept;
    ~Impl() noexcept;

    void run( Game &game ) noexcept;

    std::shared_ptr<Settings> settings;
    std::shared_ptr<BaseRenderer> renderer;
    std::shared_ptr<InputManager> inputManager;
    std::shared_ptr<AssetManager> assetManager;
  };

  GameHostX11::Impl::Impl( const std::shared_ptr<Settings> &settings ) noexcept
      : settings( settings ) {

    renderer = std::make_shared<OpenGLRenderer>( settings );
    renderer->init();

    inputManager = std::make_shared<InputManager>();

    assetManager = std::make_shared<AssetManager>();
  }

  GameHostX11::Impl::~Impl() noexcept {}

  void GameHostX11::Impl::run( Game &game ) noexcept {
    game.initialize();

    while ( !inputManager->shouldClose() ) {

      // TODO(stel): I should fix that ( calculate time between frames )
      u32 delta = 0.0f;

      inputManager->update( delta );

      renderer->submitFrame();

      game.update( delta );
      game.draw( delta );

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

}    // namespace nile::X11
