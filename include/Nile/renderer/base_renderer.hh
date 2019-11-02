/* ================================================================================
$File: base_renderer.hh
$Date: $
$Revision: $
$Creator: Rostislav Orestis Stelmach
$Notice: $
================================================================================ */

#pragma once

#include <SDL2/SDL.h>

namespace nile {

  class BaseRenderer {
  public:
    virtual ~BaseRenderer() noexcept;
    virtual void initRenderer() noexcept = 0;
    virtual void initWindow() noexcept = 0;
    virtual void init() noexcept = 0;
    virtual void submitFrame() noexcept = 0;
    virtual void endFrame() noexcept = 0;
    virtual void destroy() noexcept = 0;

    virtual SDL_Window *getWindow() noexcept = 0;
    virtual  SDL_GLContext getContext() const noexcept = 0;
  };

}    // namespace nile
