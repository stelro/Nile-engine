#include <iostream>

#include "../src/platformer.hh"
#include <Nile/core/settings.hh>
#include <Nile/platform/x11/bootstrap.hh>

#include <SDL2/SDL.h>

#include <memory>

int main() {

  auto settings = nile::Settings::Builder {}
                      .setWidth( 1024 )    // 1920
                      .setHeight( 768 )    // 920
                      .setTitle( "Nile Engine | Debugging " )
                      .setDebugMode( true )
                      .setWindowFlags( SDL_WINDOW_VULKAN | SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE )
                      .setProgramMode( nile::ProgramMode::EDITOR_MODE )
                      .build();

  nile::X11::Boostrap bootstrap( std::make_shared<nile::Settings>( settings ) );

  bootstrap.run( []( const std::shared_ptr<nile::GameHost> &gameHost ) {
    return std::make_unique<platformer::Platformer>( gameHost );
  } );

  return 0;
}
