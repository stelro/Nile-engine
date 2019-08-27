#include "renderer/opengl_renderer.hh"

#include "2d/sprite_renderer.hh"
#include "core/settings.hh"
#include "renderer/shader.hh"
#include "resource/resource_manager.hh"

#include <GL/glew.h>
#include <glm/gtc/matrix_transform.hpp>

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
    glewExperimental = GL_TRUE;
    glewInit();
    glGetError();

    glViewport( 0, 0, m_settings->getWidth(), m_settings->getHeight() );
    glEnable( GL_CULL_FACE );
    glEnable( GL_BLEND );
    glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
    m_isRunning = true;
  }

  void OpenGLRenderer::run() noexcept {
    this->initRenderer();
    this->initWindow();
    this->mainLoop();
    this->cleanUp();
  }

  void OpenGLRenderer::cleanUp() noexcept {
    ResourceManager::clear();
    SDL_DestroyWindow( m_window );
    m_window = nullptr;

    SDL_Quit();
  }

  void OpenGLRenderer::mainLoop() noexcept {

    ResourceManager::loadShader( "../shaders/sprite_vertex.glsl", "../shaders/sprite_fragment.glsl",
                                 {}, "sprite" );

    glm::mat4 projection =
        glm::ortho( 0.0f, static_cast<f32>( m_settings->getWidth() ),
                    static_cast<f32>( m_settings->getHeight() ), 0.0f, -1.0f, 1.0f );

    ResourceManager::getShader( "sprite" )->use().SetInteger( "image", 0 );
    ResourceManager::getShader( "sprite" )->use().SetMatrix4( "projection", projection );

    SpriteRenderer *renderer = new SpriteRenderer( ResourceManager::getShader( "sprite" ) );

    ResourceManager::loadTexture( "../textures/awesomeface.png", true, "face" );


    while ( m_isRunning ) {
      while ( SDL_PollEvent( &m_event ) ) {
        if ( m_event.type == SDL_QUIT ) {
          m_isRunning = false;
        }
      }

      renderer->draw( ResourceManager::getTexture( "face" ), glm::vec2( 200, 200 ),
                      glm::vec2( 300, 400 ), 45.0f, glm::vec3( 0.0f, 1.0f, 0.0f ) );

      glClearColor( 0.2f, 0.3f, 0.3f, 1.0f );
      glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
      SDL_GL_SwapWindow( m_window );
    }
  }

}    // namespace nile
