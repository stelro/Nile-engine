#pragma once

#include <memory>

namespace nile {

  class Settings;
  class BaseRenderer;
  class InputManager;

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
  };

}    // namespace nile
