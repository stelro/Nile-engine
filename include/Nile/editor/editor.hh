#pragma once

#include "Nile/core/helper.hh"
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
  };

}    // namespace nile::editor
