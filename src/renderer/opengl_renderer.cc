/* ================================================================================
$File: opengl_renderer.cc
$Date: $
$Revision: $
$Creator: Rostislav Orestis Stelmach
$Notice: $
================================================================================ */

#include "Nile/renderer/opengl_renderer.hh"

#include "Nile/2d/sprite_renderer.hh"
#include "Nile/core/settings.hh"

#include <GL/glew.h>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>


namespace nile {

  OpenGLRenderer::OpenGLRenderer( std::shared_ptr<Settings> settings ) noexcept
      : m_settings( settings ) {}

  OpenGLRenderer::~OpenGLRenderer() noexcept {
    // Empty Destructor
  }

  void OpenGLRenderer::initRenderer() noexcept {
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

      // Those attributes should be placed here, in order the renderdoc debuger can work
      SDL_GL_SetAttribute( SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE );
      SDL_GL_SetAttribute( SDL_GL_CONTEXT_MAJOR_VERSION, 3 );
      SDL_GL_SetAttribute( SDL_GL_CONTEXT_MINOR_VERSION, 3 );
      m_glContext = SDL_GL_CreateContext( m_window );

      if ( !m_glContext ) {
        log::fatal( "OpenGL Context could not be created! SDL Error: %s\n", SDL_GetError() );
      }
    }

    // Initialize GLEW
    glewExperimental = GL_TRUE;
    glewInit();
    glGetError();

    glViewport( 0, 0, m_settings->getWidth(), m_settings->getHeight() );

    glDisable( GL_DEPTH_TEST );
    glEnable( GL_CULL_FACE );
    glEnable( GL_BLEND );
    glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
    m_isRunning = true;
  }

  void OpenGLRenderer::init() noexcept {
    this->initRenderer();
    this->initWindow();
  }

  void OpenGLRenderer::destroy() noexcept {
    // ResourceManager::clear();
    SDL_DestroyWindow( m_window );
    m_window = nullptr;
    SDL_Quit();
  }

  void OpenGLRenderer::submitFrame() noexcept {
    if ( m_settings->getDebugMode() ) {
      // glClearColor( 0.2f, 0.3f, 0.3f, 1.0f );
      // TODO(stel): fix this
      glClearColor( 0.086f, 0.188f, 0.235f, 1.0f );
    } else {
      glClearColor( 0.086f, 0.188f, 0.235f, 1.0f );
    }
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
  }

  void OpenGLRenderer::endFrame() noexcept {
    SDL_GL_SwapWindow( m_window );
  }


}    // namespace nile
