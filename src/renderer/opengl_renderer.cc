#include "renderer/opengl_renderer.hh"

#include "core/settings.hh"

#include <GL/glew.h>

namespace nile {

  OpenGLRenderer::OpenGLRenderer( std::shared_ptr<Settings> settings ) noexcept
      : m_settings( settings ) {}

  OpenGLRenderer::~OpenGLRenderer() noexcept {
    // Empty Destructor
  }

  void OpenGLRenderer::initRenderer() noexcept {

    SDL_GL_SetAttribute( SDL_GL_CONTEXT_MAJOR_VERSION, 3 );
    SDL_GL_SetAttribute( SDL_GL_CONTEXT_MINOR_VERSION, 1 );
    SDL_GL_SetAttribute( SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE );
    // Initialize SDL2 ( used for window creation and keyboard input)
    ASSERT_M( ( SDL_Init( SDL_INIT_VIDEO ) >= 0 ), "Failed to init SDL" )
  }

  void OpenGLRenderer::initWindow() noexcept {

    // Create SDL Window
    m_window = SDL_CreateWindow( m_settings->getWindowTitle().c_str(), SDL_WINDOWPOS_CENTERED,
                                 SDL_WINDOWPOS_CENTERED, m_settings->getWidth(),
                                 m_settings->getHeight(), m_settings->getWindowFlags() );

    if ( !m_window ) {
      log::fatal( "SDL Window could not created! SDL Error: %s\n", SDL_GetError() );
    } else {
      // If window creation doesn't fails, then create opengl context
      m_glContext = SDL_GL_CreateContext( m_window );

      if ( !m_glContext ) {
        log::fatal( "OpenGL Context could not be created! SDL Error: %s\n", SDL_GetError() );
      }
    }

    // Initialize GLEW
    glewInit();
    m_isRunning = true;
  }

  void OpenGLRenderer::run() noexcept {
    this->initRenderer();
    this->initWindow();
    this->mainLoop();
    this->cleanUp();
  }

  void OpenGLRenderer::cleanUp() noexcept {
    SDL_DestroyWindow( m_window );
    m_window = nullptr;

    SDL_Quit();
  }

  void OpenGLRenderer::mainLoop() noexcept {
    while ( m_isRunning ) {
      while ( SDL_PollEvent( &m_event ) ) {
        if ( m_event.type == SDL_QUIT ) {
          m_isRunning = false;
        }
      }

      glClearColor( 0.2f, 0.3f, 0.3f, 1.0f );
      glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
      SDL_GL_SwapWindow( m_window );
    }
  }

}    // namespace nile
