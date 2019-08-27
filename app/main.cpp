#include <iostream>
#include <stdlib.h>

#include <core/settings.hh>
#include <renderer/opengl_renderer.hh>


int main() {

  auto settings = nile::Settings::Builder {}
                      .setWidth( 800 )
                      .setHeight( 600 )
                      .setTitle( "Basic Renderer" )
                      .setWindowFlags( SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN )
                      .build();

  nile::OpenGLRenderer renderer( std::make_shared<nile::Settings>( settings ) );
  renderer.run();

  return 0;
}
