#pragma once


namespace nile {

  class Texture2D;

  struct SpriteComponent {
    SpriteComponent()
        : texture( nullptr ) {}
    SpriteComponent( Texture2D *tex )
        : texture( tex ) {}
    // TODO(stel): fix this, make texture as a handle
    Texture2D *texture;
  };

}    // namespace nile
