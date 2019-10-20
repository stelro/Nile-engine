#pragma once 

namespace nile {

  class Texture2D;

  struct MeshComponent {
    MeshComponent()
        : texture( nullptr ) {}
    MeshComponent( Texture2D *tex )
        : texture( tex ) {}
    // TODO(stel): fix this, make texture as a handle
    Texture2D *texture;

  };

}
