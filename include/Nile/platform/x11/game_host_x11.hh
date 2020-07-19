/* ================================================================================
$File: game_host_x11.hh
$Date: $
$Revision: $
$Creator: Rostislav Orestis Stelmach
$Notice: $
================================================================================ */

#pragma once


#include "Nile/platform/game_host.hh"

#include <memory>

namespace nile {

  class Game;

  namespace X11 {

    class GameHostX11 final : public GameHost {
    private:
      // PIMPL
      class Impl;
      std::unique_ptr<Impl> impl;

    public:
      GameHostX11( const std::shared_ptr<Settings> &settings ) noexcept;
      ~GameHostX11() noexcept;

      void run( Game &game ) noexcept;

      [[nodiscard]] std::shared_ptr<Settings> get_settings() const noexcept override;
      [[nodiscard]] std::shared_ptr<BaseRenderer> get_renderer() const noexcept override;
      [[nodiscard]] std::shared_ptr<InputManager> get_input_manager() const noexcept override;
      [[nodiscard]] std::shared_ptr<AssetManager> get_asset_manager() const noexcept override;
      [[nodiscard]] std::shared_ptr<Coordinator> get_ecs_coordinator() const noexcept override;
    };

  }    // namespace X11

}    // namespace nile
