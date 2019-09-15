#include "Nile/game/game.hh"
#include "Nile/2d/2d_camera.hh"
#include "Nile/2d/sprite_renderer.hh"
#include "Nile/2d/sprite_sheet.hh"
#include "Nile/2d/spritesheet_context.hh"
#include "Nile/core/input_manager.hh"
#include "Nile/core/settings.hh"
#include "Nile/renderer/font_renderer.hh"
#include "Nile/renderer/shader.hh"
#include "Nile/resource/resource_manager.hh"

#include <GL/glew.h>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>

namespace nile {

  Game::Game( std::shared_ptr<Settings> settings ) noexcept
      : m_settings( settings ) {}

  Game::~Game() noexcept {}

  void Game::init() noexcept {


    m_animationSlot = [&]( bool value ) {
      // When the animation actually ends, we want to continue to recive events
      // as expcected, so we let know this object that the animation has ended
      m_shouldHaltTheEvents = !value;
    };

    ResourceManager::loadShader( "../shaders/sprite_vertex.glsl", "../shaders/sprite_fragment.glsl",
                                 {}, "sprite" );

    ResourceManager::loadShader( "../shaders/spritesheet_vertex.glsl",
                                 "../shaders/spritesheet_fragment.glsl", {}, "sprite_sheet" );

    auto font_shader = ResourceManager::loadShader(
        "../shaders/font_vertex.glsl", "../shaders/font_fragment.glsl", {}, "font_shader" );

    m_knightSprite = std::make_unique<SpriteSheetContext>();

    m_fontRenderer =
        std::make_unique<FontRenderer>( font_shader, m_settings, "../fonts/arial.ttf", 42 );

    m_camera = std::make_unique<Camera2D>( m_settings );
    m_camera->setScale( glm::vec2( 2.5f ) );

    auto sprite_shader = ResourceManager::getShader( "sprite" );
    auto sprite_sheet_shader = ResourceManager::getShader( "sprite_sheet" );

    sprite_shader->use().SetMatrix4( "projection", m_camera->getCameraMatrix() );
    sprite_shader->use().SetInteger( "image", 0 );

    sprite_sheet_shader->use().SetMatrix4( "projection", m_camera->getCameraMatrix() );
    sprite_sheet_shader->use().SetInteger( "image", 0 );

    m_spriteRenderer = std::make_unique<SpriteRenderer>( sprite_shader );

    ResourceManager::loadTexture( "../textures/layers/parallax-demon-woods-bg.png", true,
                                  "background" );

    ResourceManager::loadTexture( "../textures/layers/parallax-demon-woods-far-trees.png", true,
                                  "far-trees" );

    ResourceManager::loadTexture( "../textures/layers/parallax-demon-woods-mid-trees.png", true,
                                  "mid-trees" );

    ResourceManager::loadTexture( "../textures/layers/parallax-demon-woods-close-trees.png", true,
                                  "trees" );

    ResourceManager::loadTexture( "../textures/skeleton/skeleton-attack.png", true,
                                  "skeleton-attack" );

    ResourceManager::loadTexture( "../textures/Knight/knight_run.png", true, "knight_run" );
    ResourceManager::loadTexture( "../textures/Knight/knight_idle.png", true, "knight_idle" );
    ResourceManager::loadTexture( "../textures/Knight/knight_attack.png", true, "knight_attack" );
    ResourceManager::loadTexture( "../textures/Knight/knight_jump_and_fall.png", true,
                                  "knight_jump_and_fall" );
    ResourceManager::loadTexture( "../textures/Knight/knight_roll_strip.png", true,
                                  "knight_roll_strip" );
    ResourceManager::loadTexture( "../textures/Knight/knight_shield_strip.png", true,
                                  "knight_shield_strip" );


    // TODO(stel): all of this above should be moved to seperate class
    // that handles the main character!

    const f32 knight_scale_factor = 2.8f;

    m_knightSprite->addSpriteSheet( "knight_run", sprite_sheet_shader,
                                    ResourceManager::getTexture( "knight_run" ),
                                    glm::ivec2( 96, 64 ) );

    m_knightSprite->getSpriteSheet( "knight_run" )->scale( knight_scale_factor );

    m_knightSprite->addSpriteSheet( "knight_idle", sprite_sheet_shader,
                                    ResourceManager::getTexture( "knight_idle" ),
                                    glm::ivec2( 64, 64 ) );

    m_knightSprite->getSpriteSheet( "knight_idle" )->scale( knight_scale_factor );

    m_knightSprite->addSpriteSheet( "knight_attack", sprite_sheet_shader,
                                    ResourceManager::getTexture( "knight_attack" ),
                                    glm::ivec2( 144, 64 ) );

    m_knightSprite->getSpriteSheet( "knight_attack" )->scale( knight_scale_factor );

    m_knightSprite->addSpriteSheet( "knight_jump_and_fall", sprite_sheet_shader,
                                    ResourceManager::getTexture( "knight_jump_and_fall" ),
                                    glm::ivec2( 144, 64 ) );

    m_knightSprite->getSpriteSheet( "knight_jump_and_fall" )->scale( knight_scale_factor );

    m_knightSprite->addSpriteSheet( "knight_roll_strip", sprite_sheet_shader,
                                    ResourceManager::getTexture( "knight_roll_strip" ),
                                    glm::ivec2( 180, 64 ) );

    m_knightSprite->getSpriteSheet( "knight_roll_strip" )->scale( knight_scale_factor );

    m_knightSprite->addSpriteSheet( "knight_shield_strip", sprite_sheet_shader,
                                    ResourceManager::getTexture( "knight_shield_strip" ),
                                    glm::ivec2( 96, 64 ) );

    m_knightSprite->getSpriteSheet( "knight_shield_strip" )->scale( knight_scale_factor );


    m_inputManager = InputManager::getInstance();
  }

  void Game::update( [[maybe_unused]] float dt ) noexcept {


    // TODO(stel): This is hardcoded for now, soon will be fixed!
    const constexpr f32 speed = 2.4f;
    const constexpr f32 zoomScale = 0.1f;

    if ( !m_shouldHaltTheEvents ) {
      if ( m_inputManager->isKeyPressed( SDLK_d ) ) {

        m_camera->setPosition( m_camera->getPosition() - glm::vec2( speed, 0.0f ) );
        m_heroOriantaion = TextureOrientation::NE;
        m_heroState = HeroStateEnum::RUNNING;

      } else if ( m_inputManager->isKeyPressed( SDLK_a ) ) {

        m_camera->setPosition( m_camera->getPosition() + glm::vec2( speed, 0.0f ) );
        m_heroOriantaion = TextureOrientation::NW;
        m_heroState = HeroStateEnum::RUNNING;

      } else if ( m_inputManager->isKeyPressed( SDLK_h ) ) {

        m_shouldHaltTheEvents = true;
        m_animationListener.connect(
            m_knightSprite->getSpriteSheet( "knight_attack" )->animation_signal, m_animationSlot );
        m_heroState = HeroStateEnum::ATTACK;

      } else if ( m_inputManager->isKeyPressed( SDLK_SPACE ) ) {

        // TODO(stel): Fix the hero position after he performs the jump and the animation
        // has finished. Hero should move some steps forward after the sprite animation
        m_shouldHaltTheEvents = true;
        m_animationListener.connect(
            m_knightSprite->getSpriteSheet( "knight_jump_and_fall" )->animation_signal,
            m_animationSlot );
        m_heroState = HeroStateEnum::JUMP_AND_FALL;

      } else if ( m_inputManager->isKeyPressed( SDLK_j ) ) {

        // TODO(stel): same as the above ( jump animation )
        m_shouldHaltTheEvents = true;
        m_animationListener.connect(
            m_knightSprite->getSpriteSheet( "knight_roll_strip" )->animation_signal,
            m_animationSlot );
        m_heroState = HeroStateEnum::ROLL_STRIP;

      } else if ( m_inputManager->isKeyPressed( SDLK_k ) ) {

        m_shouldHaltTheEvents = true;
        m_animationListener.connect(
            m_knightSprite->getSpriteSheet( "knight_shield_strip" )->animation_signal,
            m_animationSlot );
        m_heroState = HeroStateEnum::SHIELD_STRIP;

      } else {

        m_heroState = HeroStateEnum::IDLE;
      }

      if ( m_inputManager->isKeyPressed( SDLK_r ) ) {
        m_camera->setScale( m_camera->getScale() + glm::vec2( zoomScale ) );
      }

      if ( m_inputManager->isKeyPressed( SDLK_e ) ) {
        m_camera->setScale( m_camera->getScale() - glm::vec2( zoomScale ) );
      }
    }

    m_camera->update( dt );

    if ( m_knightSprite->getSpritesOriantation() != m_heroOriantaion ) {
      // Check if the oriantation of the sprite has changed, if so then
      // update the oriantation of every sprite sheet inside sprite context
      m_knightSprite->updateSpriteOriantion( m_heroOriantaion );
    }

    // We set projection matrix to the object that are "moving"
    // since the camera is static, and we shift the world
    ResourceManager::getShader( "sprite" )
        ->use()
        .SetMatrix4( "projection", m_camera->getCameraMatrix() );
  }

  void Game::render( [[maybe_unused]] float dt ) noexcept {
    m_fontRenderer->renderText( "test", 5.0f, 5.0f, 1.0f, glm::vec3( 1.0f, 1.0f, 1.0f ) );

    // log::print( m_shouldHaltTheEvents ? "true\n" : "false\n" );

    // TODO(stel): hardcoded for now, in the near feature this will be fixed!
    const constexpr auto tileWidth = 1078;
    const constexpr auto tileHeight = 224;
    const constexpr auto runs = 4;
    const constexpr auto on_screen_height = 20;

    for ( int i = 0; i < runs; i++ ) {
      // The background layer
      m_spriteRenderer->draw(
          ResourceManager::getTexture( "background" ), glm::vec2( i * tileWidth, on_screen_height ),
          glm::vec2( tileWidth, tileHeight ), 0.0f, glm::vec3( 1.0f, 1.0f, 1.0f ) );
    }

    for ( int i = 0; i < runs; i++ ) {
      // Layer 3 ( layer 3 moves faster than layer 2 )
      m_spriteRenderer->draw(
          ResourceManager::getTexture( "far-trees" ),
          glm::vec2( ( i * tileWidth ) + m_camera->getPosition().x * 0.3f, on_screen_height ),
          glm::vec2( tileWidth, tileHeight ), 0.0f, glm::vec3( 1.0f, 1.0f, 1.0f ) );
    }

    for ( int i = 0; i < runs; i++ ) {
      // Layer 2 ( layer 2 moves slower than layer 3 and faster than layer 1 )
      m_spriteRenderer->draw(
          ResourceManager::getTexture( "mid-trees" ),
          glm::vec2( ( i * tileWidth ) + m_camera->getPosition().x * 0.2f, on_screen_height ),
          glm::vec2( tileWidth, tileHeight ), 0.0f, glm::vec3( 1.0f, 1.0f, 1.0f ) );
    }

    for ( int i = 0; i < runs; i++ ) {
      // layer 1 ( the main layer )
      m_spriteRenderer->draw(
          ResourceManager::getTexture( "trees" ), glm::vec2( ( i * tileWidth ), on_screen_height ),
          glm::vec2( tileWidth, tileHeight ), 0.0f, glm::vec3( 1.0f, 1.0f, 1.0f ) );
    }

    // TODO(stel): make it a switch statement, instead of this if/else chaos

    const i32 knight_height_offset = 180;

    if ( m_heroState == HeroStateEnum::IDLE ) {
      m_knightSprite->playAnimation(
          "knight_idle", glm::vec2( 1, m_settings->getHeight() - knight_height_offset ) );
    } else if ( m_heroState == HeroStateEnum::RUNNING ) {
      m_knightSprite->playAnimation(
          "knight_run", glm::vec2( 1, m_settings->getHeight() - knight_height_offset ) );
    } else if ( m_heroState == HeroStateEnum::ATTACK ) {
      m_knightSprite->playAnimationAndHalt(
          "knight_attack", glm::vec2( 1, m_settings->getHeight() - knight_height_offset ) );
    } else if ( m_heroState == HeroStateEnum::JUMP_AND_FALL ) {
      m_knightSprite->playAnimationAndHalt(
          "knight_jump_and_fall", glm::vec2( 1, m_settings->getHeight() - knight_height_offset ) );
    } else if ( m_heroState == HeroStateEnum::ROLL_STRIP ) {
      m_knightSprite->playAnimationAndHalt(
          "knight_roll_strip", glm::vec2( 1, m_settings->getHeight() - knight_height_offset ) );
    } else if ( m_heroState == HeroStateEnum::SHIELD_STRIP ) {
      m_knightSprite->playAnimationAndHalt(
          "knight_shield_strip", glm::vec2( 1, m_settings->getHeight() - knight_height_offset ) );
    }
  }

}    // namespace nile
