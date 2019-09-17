#include "main_character.hh"
#include <Nile/renderer/shader.hh>
#include <Nile/resource/resource_manager.hh>

namespace platformer {

  using namespace nile;

  MainCharacter::MainCharacter( const std::shared_ptr<nile::Settings> &settings,
                                const std::shared_ptr<InputManager> &inputManager,
                                const std::shared_ptr<Camera2D> &camera ) noexcept
      : m_settings( settings )
      , m_inputManager( inputManager )
      , m_spriteContext( std::make_unique<SpriteSheetContext>() )
      , m_camera( camera ) {}


  MainCharacter::~MainCharacter() noexcept {}

  void MainCharacter::initialize() noexcept {


    m_animationSlot = [&]( bool value ) {
      // When the animation actually ends, we want to continue to recive events
      // as expcected, so we let know this object that the animation has ended
      m_shouldHaltTheEvents = !value;
    };

    auto sprite_sheet_shader = ResourceManager::loadShader(
        "../assets/shaders/spritesheet_vertex.glsl", "../assets/shaders/spritesheet_fragment.glsl",
        {}, "sprite_sheet" );

    ResourceManager::loadTexture( "../assets/textures/Knight/knight_run.png", true, "knight_run" );
    ResourceManager::loadTexture( "../assets/textures/Knight/knight_idle.png", true,
                                  "knight_idle" );
    ResourceManager::loadTexture( "../assets/textures/Knight/knight_attack.png", true,
                                  "knight_attack" );
    ResourceManager::loadTexture( "../assets/textures/Knight/knight_jump_and_fall.png", true,
                                  "knight_jump_and_fall" );
    ResourceManager::loadTexture( "../assets/textures/Knight/knight_roll_strip.png", true,
                                  "knight_roll_strip" );
    ResourceManager::loadTexture( "../assets/textures/Knight/knight_shield_strip.png", true,
                                  "knight_shield_strip" );


    sprite_sheet_shader->use().SetMatrix4( "projection", m_camera->getCameraMatrix() );
    sprite_sheet_shader->use().SetInteger( "image", 0 );

    const f32 knight_scale_factor = 2.8f;

    m_spriteContext->addSpriteSheet( "knight_run", sprite_sheet_shader,
                                     ResourceManager::getTexture( "knight_run" ),
                                     glm::ivec2( 96, 64 ) );

    m_spriteContext->getSpriteSheet( "knight_run" )->scale( knight_scale_factor );

    m_spriteContext->addSpriteSheet( "knight_idle", sprite_sheet_shader,
                                     ResourceManager::getTexture( "knight_idle" ),
                                     glm::ivec2( 64, 64 ) );

    m_spriteContext->getSpriteSheet( "knight_idle" )->scale( knight_scale_factor );

    m_spriteContext->addSpriteSheet( "knight_attack", sprite_sheet_shader,
                                     ResourceManager::getTexture( "knight_attack" ),
                                     glm::ivec2( 144, 64 ) );

    m_spriteContext->getSpriteSheet( "knight_attack" )->scale( knight_scale_factor );

    m_spriteContext->addSpriteSheet( "knight_jump_and_fall", sprite_sheet_shader,

                                     ResourceManager::getTexture( "knight_jump_and_fall" ),
                                     glm::ivec2( 144, 64 ) );

    m_spriteContext->getSpriteSheet( "knight_jump_and_fall" )->scale( knight_scale_factor );

    m_spriteContext->addSpriteSheet( "knight_roll_strip", sprite_sheet_shader,
                                     ResourceManager::getTexture( "knight_roll_strip" ),
                                     glm::ivec2( 180, 64 ) );

    m_spriteContext->getSpriteSheet( "knight_roll_strip" )->scale( knight_scale_factor );

    m_spriteContext->addSpriteSheet( "knight_shield_strip", sprite_sheet_shader,
                                     ResourceManager::getTexture( "knight_shield_strip" ),
                                     glm::ivec2( 96, 64 ) );

    m_spriteContext->getSpriteSheet( "knight_shield_strip" )->scale( knight_scale_factor );
    
  }

  void MainCharacter::update( f32 deltaTime ) noexcept {

    // TODO(stel): This is hardcoded for now, soon will be fixed!
    const constexpr f32 speed = 2.4f;


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
            m_spriteContext->getSpriteSheet( "knight_attack" )->animation_signal, m_animationSlot );
        m_heroState = HeroStateEnum::ATTACK;

      } else if ( m_inputManager->isKeyPressed( SDLK_SPACE ) ) {

        // TODO(stel): Fix the hero position after he performs the jump and the animation
        // has finished. Hero should move some steps forward after the sprite animation
        m_shouldHaltTheEvents = true;
        m_animationListener.connect(
            m_spriteContext->getSpriteSheet( "knight_jump_and_fall" )->animation_signal,
            m_animationSlot );
        m_heroState = HeroStateEnum::JUMP_AND_FALL;

      } else if ( m_inputManager->isKeyPressed( SDLK_j ) ) {

        // TODO(stel): same as the above ( jump animation )
        m_shouldHaltTheEvents = true;
        m_animationListener.connect(
            m_spriteContext->getSpriteSheet( "knight_roll_strip" )->animation_signal,
            m_animationSlot );
        m_heroState = HeroStateEnum::ROLL_STRIP;

      } else if ( m_inputManager->isKeyPressed( SDLK_k ) ) {

        m_shouldHaltTheEvents = true;
        m_animationListener.connect(
            m_spriteContext->getSpriteSheet( "knight_shield_strip" )->animation_signal,
            m_animationSlot );
        m_heroState = HeroStateEnum::SHIELD_STRIP;

      } else {
        m_heroState = HeroStateEnum::IDLE;
      }

      if ( m_spriteContext->getSpritesOriantation() != m_heroOriantaion ) {
        // Check if the oriantation of the sprite has changed, if so then
        // update the oriantation of every sprite sheet inside sprite context
        m_spriteContext->updateSpriteOriantion( m_heroOriantaion );
      }
    }
  }

  void MainCharacter::draw( f32 deltaTime ) noexcept {
    // TODO(stel): make it a switch statement, instead of this if/else chaos

    const i32 knight_height_offset = 180;

    if ( m_heroState == HeroStateEnum::IDLE ) {
      m_spriteContext->playAnimation(
          "knight_idle", glm::vec2( 1, m_settings->getHeight() - knight_height_offset ) );
    } else if ( m_heroState == HeroStateEnum::RUNNING ) {
      m_spriteContext->playAnimation(
          "knight_run", glm::vec2( 1, m_settings->getHeight() - knight_height_offset ) );
    } else if ( m_heroState == HeroStateEnum::ATTACK ) {
      m_spriteContext->playAnimationAndHalt(
          "knight_attack", glm::vec2( 1, m_settings->getHeight() - knight_height_offset ) );
    } else if ( m_heroState == HeroStateEnum::JUMP_AND_FALL ) {
      m_spriteContext->playAnimationAndHalt(
          "knight_jump_and_fall", glm::vec2( 1, m_settings->getHeight() - knight_height_offset ) );
    } else if ( m_heroState == HeroStateEnum::ROLL_STRIP ) {
      m_spriteContext->playAnimationAndHalt(
          "knight_roll_strip", glm::vec2( 1, m_settings->getHeight() - knight_height_offset ) );
    } else if ( m_heroState == HeroStateEnum::SHIELD_STRIP ) {
      m_spriteContext->playAnimationAndHalt(
          "knight_shield_strip", glm::vec2( 1, m_settings->getHeight() - knight_height_offset ) );
    }
  }

}    // namespace platformer
