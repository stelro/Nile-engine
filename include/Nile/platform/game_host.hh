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

  class GameHost {
  protected:
    std::shared_ptr<Settings> m_settings;

  public:
    GameHost( const std::shared_ptr<Settings> &settings ) noexcept
        : m_settings( settings ) {}
    virtual ~GameHost() noexcept {}

    [[nodiscard]] virtual std::shared_ptr<Settings> getSettings() const noexcept = 0;

    [[nodiscard]] virtual std::shared_ptr<BaseRenderer> getRenderer() const noexcept = 0;

    [[nodiscard]] virtual std::shared_ptr<InputManager> getInputManager() const noexcept = 0;

    [[nodiscard]] virtual std::shared_ptr<AssetManager> getAssetManager() const noexcept = 0;
  };

}    // namespace nile
