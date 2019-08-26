#pragma once

#include "core/nile.hh"
#include "renderer/base_renderer.hh"
#include <SDL2/SDL.h>
#include <memory>

namespace nile {

  class Settings;

  class OpenGLRenderer : public BaseRenderer {
  private:
    // Settings object ( keeps all configuration for the rendering engine)
    std::shared_ptr<Settings> m_settings;
    // The main SDL window
    SDL_Window *m_window = nullptr;
    SDL_GLContext m_glContext;
    SDL_Event m_event;

    // The main bool flag that keeps the main loop runing
    bool m_isRunning = false;
    // Intialize the OpenGL renderer
    void initRenderer() noexcept override;
    // Create and initialize the main Window ( SDL )
    void initWindow() noexcept override;
    // Run the renderer, this method calls all init functions
    void cleanUp() noexcept override;
    // the main renderer loop
    // @note: maybe thransfer this elsewere?
    void mainLoop() noexcept override;
  public:
    explicit OpenGLRenderer( std::shared_ptr<Settings> settings ) noexcept;
    ~OpenGLRenderer() noexcept;
    void run() noexcept override;
    // cleanup and delete all resources
  };

}    // namespace nile
