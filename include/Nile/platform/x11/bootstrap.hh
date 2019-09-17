#pragma once

#include <functional>
#include <memory>

namespace nile {

  class Game;
  class GameHost;
  class Settings;

  namespace X11 {

    class Boostrap {
    private:
      std::shared_ptr<Settings> m_settings;

    public:
      Boostrap( const std::shared_ptr<Settings> &settings ) noexcept;
      void run( const std::function<std::unique_ptr<Game>( const std::shared_ptr<GameHost> & )>
                    &crateApp ) noexcept;
    };

  }    // namespace X11
}    // namespace nile
