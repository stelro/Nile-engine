#include "game/game.hh"
#include "2d/sprite_renderer.hh"
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

    glm::mat4 projection =
        glm::ortho( 0.0f, static_cast<f32>( m_settings->getWidth() ),
                    static_cast<f32>( m_settings->getHeight() ), 0.0f, -1.0f, 1.0f );

    auto sprite_shader = ResourceManager::getShader( "sprite" );
    sprite_shader->use().SetInteger( "image", 0 );
    sprite_shader->use().SetMatrix4( "projection", projection );

    m_spriteRenderer = std::make_unique<SpriteRenderer>( sprite_shader );

    ResourceManager::loadTexture( "../textures/background.png", true, "background" );

    m_inputManager = InputManager::getInstance();
  }

  void Game::update( [[maybe_unused]] float dt ) noexcept {}

  void Game::render( [[maybe_unused]] float dt ) noexcept {
    m_spriteRenderer->draw( ResourceManager::getTexture( "background" ), glm::vec2( 1, 1 ),
                            glm::vec2( 1078, 224 ), 0.0f, glm::vec3( 1.0f, 1.0f, 1.0f ) );
  }

}    // namespace nile
