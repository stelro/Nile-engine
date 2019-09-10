#include "game/game.hh"
#include "2d/2d_camera.hh"
#include "2d/sprite_renderer.hh"
#include "2d/sprite_sheet.hh"
#include "core/input_manager.hh"
#include "core/settings.hh"
#include "renderer/shader.hh"
#include "resource/resource_manager.hh"

#include <GL/glew.h>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>


namespace nile {

  Game::Game( std::shared_ptr<Settings> settings ) noexcept
      : m_settings( settings ) {}

  Game::~Game() noexcept {}

  void Game::init() noexcept {

    ResourceManager::loadShader( "../shaders/sprite_vertex.glsl", "../shaders/sprite_fragment.glsl",
                                 {}, "sprite" );

    ResourceManager::loadShader( "../shaders/spritesheet_vertex.glsl",
                                 "../shaders/spritesheet_fragment.glsl", {}, "sprite_sheet" );

    m_camera = std::make_unique<Camera2D>( m_settings );
    m_camera->setScale( glm::vec2( 1.5f ) );

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


    m_spriteSheet = std::make_unique<SpriteSheet>(
        sprite_sheet_shader, ResourceManager::getTexture( "knight_run" ), glm::ivec2( 96, 64 ) );
    m_spriteSheet->scale( 2.4f );

    m_inputManager = InputManager::getInstance();
  }

  void Game::update( [[maybe_unused]] float dt ) noexcept {

    f32 speed = 4.2f;
    f32 zoomScale = 0.1f;

    if ( m_inputManager->isKeyPressed( SDLK_d ) ) {
      m_camera->setPosition( m_camera->getPosition() - glm::vec2( speed, 0.0f ) );
    }

    if ( m_inputManager->isKeyPressed( SDLK_a ) ) {
      m_camera->setPosition( m_camera->getPosition() + glm::vec2( speed, 0.0f ) );
    }


    // if ( m_inputManager->isKeyPressed( SDLK_w ) ) {
    //   m_camera->setPosition( m_camera->getPosition() + glm::vec2( 0.0f, speed ) );
    // }
    //
    // if ( m_inputManager->isKeyPressed( SDLK_s ) ) {
    //   m_camera->setPosition( m_camera->getPosition() - glm::vec2( 0.0f, speed ) );
    // }

    if ( m_inputManager->isKeyPressed( SDLK_r ) ) {
      m_camera->setScale( m_camera->getScale() + glm::vec2( zoomScale ) );
    }

    if ( m_inputManager->isKeyPressed( SDLK_e ) ) {
      m_camera->setScale( m_camera->getScale() - glm::vec2( zoomScale ) );
    }
    m_camera->update( dt );

    // We set projection matrix to the object that are "moving"
    // since the camera is static, and we shift the world
    ResourceManager::getShader( "sprite" )
        ->use()
        .SetMatrix4( "projection", m_camera->getCameraMatrix() );
  }

  void Game::render( [[maybe_unused]] float dt ) noexcept {

    const auto tileWidth = 1078;
    const auto tileHeight = 224;
    const auto runs = 4;


    for ( int i = 0; i < runs; i++ ) {
      m_spriteRenderer->draw( ResourceManager::getTexture( "background" ),
                              glm::vec2( i * tileWidth, m_settings->getHeight() / 5 ),
                              glm::vec2( tileWidth, tileHeight ), 0.0f,
                              glm::vec3( 1.0f, 1.0f, 1.0f ) );
    }

    for ( int i = 0; i < runs; i++ ) {
      m_spriteRenderer->draw( ResourceManager::getTexture( "far-trees" ),
                              glm::vec2( ( i * tileWidth ) + m_camera->getPosition().x * 0.3f,
                                         m_settings->getHeight() / 5 ),
                              glm::vec2( tileWidth, tileHeight ), 0.0f,
                              glm::vec3( 1.0f, 1.0f, 1.0f ) );
    }

    for ( int i = 0; i < runs; i++ ) {
      m_spriteRenderer->draw( ResourceManager::getTexture( "mid-trees" ),
                              glm::vec2( ( i * tileWidth ) + m_camera->getPosition().x * 0.5f,
                                         m_settings->getHeight() / 5 ),
                              glm::vec2( tileWidth, tileHeight ), 0.0f,
                              glm::vec3( 1.0f, 1.0f, 1.0f ) );
    }

    for ( int i = 0; i < runs; i++ ) {
      m_spriteRenderer->draw( ResourceManager::getTexture( "trees" ),
                              glm::vec2( ( i * tileWidth ), m_settings->getHeight() / 5 ),
                              glm::vec2( tileWidth, tileHeight ), 0.0f,
                              glm::vec3( 1.0f, 1.0f, 1.0f ) );
    }


    // m_spriteSheet->draw_frame( glm::vec2( 1, m_settings->getHeight() - 220 ), 3 );
    m_spriteSheet->playAnimation( glm::vec2( 1, m_settings->getHeight() - 220 ), 40 );
  }

}    // namespace nile
