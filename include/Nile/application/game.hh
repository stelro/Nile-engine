/* ================================================================================
$File: game.hh
$Date: $
$Revision: 09 Oct 2019 $
$Creator: Rostislav Orestis Stelmach
$Notice: $
================================================================================ */

#pragma once

#include "Nile/core/helper.hh"
#include "Nile/core/types.hh"

namespace nile {

  class Game {
  public:
    Game() noexcept = default;
    virtual ~Game() noexcept = default;

    NILE_DISABLE_COPY( Game )
    NILE_DISABLE_MOVE( Game )

    virtual void initialize() noexcept = 0;

    virtual void draw( f32 deltaTime ) noexcept = 0;

    virtual void update( f32 deltaTime ) noexcept = 0;
  };

}    // namespace nile
