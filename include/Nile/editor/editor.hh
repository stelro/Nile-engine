#pragma once

#include "Nile/core/helper.hh"
#include "Nile/core/types.hh"
#include <SDL2/SDL.h>

#include <memory>

namespace nile::editor {

  class Editor {
  private:
    // PIMPL
    class EditorImpl;
    std::unique_ptr<EditorImpl> m_impl;

  public:
    Editor( SDL_Window *window, SDL_GLContext context ) noexcept;
    ~Editor() noexcept;

    Editor( Editor &&rhs ) noexcept;
    Editor &operator=( Editor &&rhs ) noexcept;

    NILE_DISABLE_COPY( Editor )

    void render( float dt ) noexcept;
    void update( float dt ) noexcept;

    // Setters for Overlay Info Struct
    void setFps( f32 fps ) noexcept;
    void setEntities( u32 entities ) noexcept;
    void setComponents( u32 components ) noexcept;
    void setEcsSystems( u32 systems ) noexcept;
    void setUptime(u32 uptime) noexcept;
    void setLoadersCount(u32 loaders) noexcept;
  };

}    // namespace nile::editor
