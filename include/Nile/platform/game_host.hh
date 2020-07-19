/* ================================================================================
$File: game_host.hh
$Date: $
$Revision: $
$Creator: Rostislav Orestis Stelmach
$Notice: $
================================================================================ */

#pragma once

#include <memory>

namespace nile {

  class Settings;
  class BaseRenderer;
  class InputManager;
  class AssetManager;
  class Coordinator;

  class GameHost {
  protected:
    std::shared_ptr<Settings> settings_;

  public:
    GameHost( const std::shared_ptr<Settings> &settings ) noexcept
        : settings_( settings ) {}
    virtual ~GameHost() noexcept {}

    [[nodiscard]] virtual std::shared_ptr<Settings> get_settings() const noexcept = 0;
    [[nodiscard]] virtual std::shared_ptr<BaseRenderer> get_renderer() const noexcept = 0;
    [[nodiscard]] virtual std::shared_ptr<InputManager> get_input_manager() const noexcept = 0;
    [[nodiscard]] virtual std::shared_ptr<AssetManager> get_asset_manager() const noexcept = 0;
    [[nodiscard]] virtual std::shared_ptr<Coordinator> get_ecs_coordinator() const noexcept = 0;
  };

}    // namespace nile
