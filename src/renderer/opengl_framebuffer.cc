#include "Nile/renderer/opengl_framebuffer.hh"
#include "Nile/core/assert.hh"
#include "Nile/core/settings.hh"

#include <GL/glew.h>

namespace nile {

  OpenglFramebuffer::OpenglFramebuffer( const std::shared_ptr<Settings> &settings ) noexcept
      : m_settings( settings ) {
    this->initialize();
  }

  OpenglFramebuffer::~OpenglFramebuffer() noexcept {

    glDeleteFramebuffers( 1, &m_fbo );
  }

  void OpenglFramebuffer::initialize() noexcept {


    glGenFramebuffers( 1, &m_fbo );

    glBindFramebuffer( GL_FRAMEBUFFER, m_fbo );


    glGenTextures( 1, &m_textureColorBuffer );
    glBindTexture( GL_TEXTURE_2D, m_textureColorBuffer );

    auto const screen_width = m_settings->getWidth();
    auto const screen_height = m_settings->getHeight();

    glTexImage2D( GL_TEXTURE_2D, 0, GL_RGB, screen_width, screen_height, 0, GL_RGB,
                  GL_UNSIGNED_BYTE, nullptr );

    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
    glBindTexture( GL_TEXTURE_2D, 0 );

    // Attach the texture to the framebuffer
    glFramebufferTexture2D( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
                            m_textureColorBuffer, 0 );

    // Renderbuffer object
    glGenRenderbuffers( 1, &m_rbo );
    glBindRenderbuffer( GL_RENDERBUFFER, m_rbo );

    // Create a depth and stencil renderbuffer objerct
    glRenderbufferStorage( GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, screen_width, screen_height );

    glFramebufferRenderbuffer( GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER,
                               m_rbo );

    glBindRenderbuffer( GL_RENDERBUFFER, 0 );

    ASSERT_M( glCheckFramebufferStatus( GL_FRAMEBUFFER ) == GL_FRAMEBUFFER_COMPLETE,
              "Failed to create framebuffer, framebuffer is not complete!\n" );


    glBindFramebuffer( GL_FRAMEBUFFER, 0 );
  }

  void OpenglFramebuffer::bind() noexcept {
    glBindFramebuffer( GL_FRAMEBUFFER, m_fbo );
  }

  void OpenglFramebuffer::unbind() noexcept {
    glBindFramebuffer( GL_FRAMEBUFFER, 0 );
  }

  void OpenglFramebuffer::bindCollorBuffer() noexcept {
    glBindTexture( GL_TEXTURE_2D, m_textureColorBuffer );
  }

  void OpenglFramebuffer::prepareQuad() noexcept {

    float quadVertices[] = {-1.0f, 1.0f,  0.0f, 1.0f, -1.0f, -1.0f, 0.0f, 0.0f,
                            1.0f,  -1.0f, 1.0f, 0.0f, -1.0f, 1.0f,  0.0f, 1.0f,
                            1.0f,  -1.0f, 1.0f, 0.0f, 1.0f,  1.0f,  1.0f, 1.0f};

    glGenVertexArrays( 1, &m_quadVao );
    glGenBuffers( 1, &m_quadVbo );
    glBindVertexArray( m_quadVao );
    glBindBuffer( GL_ARRAY_BUFFER, m_quadVbo );
    glBufferData( GL_ARRAY_BUFFER, sizeof( quadVertices ), &quadVertices, GL_STATIC_DRAW );
    glEnableVertexAttribArray( 0 );
    glVertexAttribPointer( 0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof( float ), ( void * )0 );
    glEnableVertexAttribArray( 1 );
    glVertexAttribPointer( 1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof( float ),
                           ( void * )( 2 * sizeof( float ) ) );
    m_readyToRender = true;
  }

  void OpenglFramebuffer::submitFrame() noexcept {

    ASSERT_M( m_readyToRender,
              "Called submitFrame() on the Frambuffer, without prepareQuad() frist\n" );
    glBindVertexArray( m_quadVao );
    this->bindCollorBuffer();
    glDrawArrays( GL_TRIANGLES, 0, 6 );
  }

}    // namespace nile
