#include <iostream>

#include "../src/platformer.hh"
#include <Nile/core/settings.hh>
#include <Nile/platform/x11/bootstrap.hh>

#include <SDL2/SDL.h>

#include <memory>

int main() {

  auto settings = nile::Settings::Builder {}
                      .setWidth( 1024 )
                      .setHeight( 600 )
                      .setTitle( "Deathmarch v0.1" )
                      .setDebugMode( true )
                      .setWindowFlags( SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN )
                      .build();

  nile::X11::Boostrap bootstrap( std::make_shared<nile::Settings>( settings ) );

  bootstrap.run( []( const std::shared_ptr<nile::GameHost> &gameHost ) {
    return std::make_unique<platformer::Platformer>( gameHost );
  } );

  return 0;
}
