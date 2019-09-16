#include <iostream>
#include <stdlib.h>

#include <Nile/core/input_manager.hh>
#include <Nile/core/settings.hh>
#include <Nile/game/game.hh>
#include <Nile/renderer/opengl_renderer.hh>

using nile::u32;

#include <SDL2/SDL.h>

int main() {
  //
  // SDL_Event e;
  //
  // SDL_PollEvent(&e);
  //
  // SDL_Window* window = SDL_CreateWindow("hello", 0, 0, 1024, 768, 9);
  //
  // window = nullptr;
  //
  auto settings = nile::Settings::Builder {}
                      .setWidth( 1024 )
                      .setHeight( 600 )
                      .setTitle( "Deathmarch v0.1" )
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
