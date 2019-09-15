#include "Nile/2d/sprite_sheet.hh"
#include "Nile/renderer/shader.hh"
#include "Nile/renderer/texture2d.hh"
#include <GL/glew.h>
#include <SDL2/SDL.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace nile {

  SpriteSheet::SpriteSheet( const std::shared_ptr<Shader> &shader,
                            const std::shared_ptr<Texture2D> &texture,
                            const glm::ivec2 &spriteDimensions ) noexcept
      : m_shader( shader )
      , m_texture( texture )
      , m_spriteDimensions( spriteDimensions )
      , m_color( glm::vec3( 1.0f, 1.0f, 1.0f ) )
      , m_size( spriteDimensions )
      , m_rotateAngle( 0.0f )
      , m_numberOfColumns( 0 )
      , m_numberOfRows( 0 )
      , m_animationDelay( 0 )
      , m_currentFrame( 0 )
      , m_textureOrientation( TextureOrientation::NE ) {

    // OpenGL, generate vertex attribute object
    // and vertex buffer objects
    glGenVertexArrays( 1, &m_quadVAO );
    glGenBuffers( 1, &m_quadVBO );

    this->initRenderData();
  }

  SpriteSheet::~SpriteSheet() noexcept {
    glDeleteVertexArrays( 1, &this->m_quadVAO );
  }

  void SpriteSheet::draw_frame( const glm::vec2 &position, [[maybe_unused]] u32 frame_index,
                                [[maybe_unused]] u32 speed ) noexcept {

    this->m_shader->use();

    glm::mat4 model = glm::mat4( 1.0f );
    model = glm::translate( model, glm::vec3( position, 0.0f ) );

    // Because we specified the quad's vertices with (0,0) as the top-left coordinate
    // of the quad, all rotations will rotate around this point of (0.0). Basically the
    // origin of rotation is the top-left of the quad which produces undesirable results.
    // so we move the origin of rotation to the centre of the quad, so the quad
    // neatly rotates around this origin.
    model = glm::translate( model, glm::vec3( 0.5f * m_size.x, 0.5f * m_size.y, 0.0f ) );
    model = glm::rotate( model, m_rotateAngle, glm::vec3( 0.0f, 0.0f, 1.0f ) );
    model = glm::translate( model, glm::vec3( -0.5f * m_size.x, -0.5f * m_size.y, 0.0f ) );

    model = glm::scale( model, glm::vec3( m_size, 1.0f ) );

    const i32 col = frame_index % m_numberOfColumns;
    const i32 row = m_numberOfRows - 1 - frame_index / m_numberOfRows;
    auto s = ( float )col / ( float )m_numberOfColumns;
    auto t = ( float )row / ( float )m_numberOfRows;

    i32 rows = m_numberOfRows;
    i32 cols = m_numberOfColumns;

      switch ( m_textureOrientation ) {
      case TextureOrientation::NE:
        // the sprite is facing right/up ( normal oriantation )
        cols = m_numberOfColumns;
        rows = m_numberOfRows;
        break;
      case TextureOrientation::SW:
        // the sprite is facing left/down ( flipped to the left and down)
        cols = -m_numberOfColumns;
        rows = -m_numberOfRows;
        break;
      case TextureOrientation::SE:
        // the sprite is facing right/down ( flipped to down )
        cols = m_numberOfColumns;
        rows = -m_numberOfRows;
        break;
      case TextureOrientation::NW:
        // The sprite is facing left/up ( flipped to the left )
        cols = -m_numberOfColumns;
        rows = m_numberOfRows;
        break;
      default:
        break;
    }

    this->m_shader->SetVector2f( "st", glm::vec2( s, t ) );
    this->m_shader->SetVector2f( "size",
                                 glm::vec2( static_cast<f32>( rows ), static_cast<f32>( cols ) ) );
    this->m_shader->SetMatrix4( "model", model );
    this->m_shader->SetVector3f( "spriteColor", m_color );

    glActiveTexture( GL_TEXTURE0 );
    m_texture->bind();

    glBindVertexArray( this->m_quadVAO );

    glDrawArrays( GL_TRIANGLES, 0, 6 );
    glBindVertexArray( 0 );
  }

  void SpriteSheet::playAnimation( [[maybe_unused]] const glm::vec2 &position,
                                   u32 speed ) noexcept {
    // This is used to perform the actual animation
    // of the sprite
    if ( m_animationDelay + speed < SDL_GetTicks() ) {
      m_currentFrame++;
      if ( m_currentFrame >= m_numberOfColumns ) {
        m_currentFrame = 0;
      }

      m_animationDelay = SDL_GetTicks();
    }
    this->draw_frame( position, m_currentFrame, speed );
  }

  void SpriteSheet::playAnimationAndHalt( [[maybe_unused]] const glm::vec2 &position,
                                          u32 speed ) noexcept {

    playAnimation( position, speed );

    if ( m_currentFrame == ( m_numberOfColumns - 1 ) ) {
      // emit a signal, that the current animation has finished
      // and every frame has played, in meanwhile while the animation is on progress
      // we should prevent any events to happen
      animation_signal.emit( true );
    }
  }

  void SpriteSheet::initRenderData() noexcept {

    m_numberOfColumns = m_texture->getWidth() / m_spriteDimensions.x;
    m_numberOfRows = m_texture->getHeight() / m_spriteDimensions.y;

    // bottom-left corner is (0.0f, 0.0f) and top right corner is
    // (1.0f, 1.0f). The order is clockwise
    f32 vertices[] = {0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,

                      0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f};


    glBindBuffer( GL_ARRAY_BUFFER, m_quadVBO );
    glBufferData( GL_ARRAY_BUFFER, sizeof( vertices ), vertices, GL_STATIC_DRAW );

    glBindVertexArray( this->m_quadVAO );

    glVertexAttribPointer( 0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof( f32 ), ( void * )0 );
    glEnableVertexAttribArray( 0 );

    glBindBuffer( GL_ARRAY_BUFFER, 0 );
    glBindVertexArray( 0 );
  }

  void SpriteSheet::setColor( const glm::vec3 &color ) noexcept {
    this->m_color = color;
  }

  void SpriteSheet::setSize( const glm::vec2 &size ) noexcept {
    this->m_size = size;
  }

  void SpriteSheet::setRotationAngle( f32 angle ) noexcept {
    this->m_rotateAngle = angle;
  }

  void SpriteSheet::scale( f32 scalar ) noexcept {
    this->m_size = this->m_size * scalar;
  }

  void SpriteSheet::setTextureOrientation( TextureOrientation orientation ) noexcept {
    m_textureOrientation = orientation;
  }

}    // namespace nile
