#include <iostream>
#include <stdlib.h>

#include <core/input_manager.hh>
#include <core/settings.hh>
#include <game/game.hh>
#include <renderer/opengl_renderer.hh>

using nile::u32;

int main() {

  auto settings = nile::Settings::Builder {}
                      .setWidth( 800 )
                      .setHeight( 600 )
                      .setTitle( "Basic Renderer" )
                      .setDebugMode( true )
                      .setWindowFlags( SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN )
                      .build();

  auto settings_ptr = std::make_shared<nile::Settings>( settings );
  nile::OpenGLRenderer renderer( settings_ptr );

  renderer.init();

  auto inputManager = nile::InputManager::getInstance();

  nile::Game game( settings_ptr );
  game.init();


  // Main renderer / game loop
  while ( !inputManager->shouldClose() ) {

    if ( inputManager->isKeyPressed( SDLK_ESCAPE ) ) {
      inputManager->terminateEngine();
    }

    u32 delta = 0.0f;

    inputManager->update( delta );

    renderer.submitFrame();

    // Game methods
    game.update( delta );
    game.render( delta );

    renderer.endFrame();
  }

  nile::InputManager::destroy();

  return 0;
}
