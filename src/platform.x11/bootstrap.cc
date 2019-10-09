/* ================================================================================
$File: bootstrap.cc
$Date: $
$Revision: $
$Creator: Rostislav Orestis Stelmach
$Notice: $
================================================================================ */

#include "Nile/platform/x11/bootstrap.hh"
#include "Nile/application/game.hh"
#include "Nile/platform/x11/game_host_x11.hh"
#include "Nile/core/assert.hh"

namespace nile::X11 {

  Boostrap::Boostrap( const std::shared_ptr<Settings> &settings ) noexcept
      : m_settings( settings ) {}

  void
  Boostrap::run( const std::function<std::unique_ptr<Game>( const std::shared_ptr<GameHost> & )>
                     &createApp ) noexcept {

    ASSERT_M( createApp, "createApp function is NULL!" );

    std::shared_ptr<GameHostX11> gameHost;
    std::unique_ptr<Game> game;

    // TODO(stel): We should consider other OSs too
    gameHost = std::make_shared<GameHostX11>( m_settings );

    game = createApp( gameHost );

    ASSERT_M( game, "Game is NULL" );

    if ( game ) {
      gameHost->run( *game );
    }
  }

}    // namespace nile::X11
