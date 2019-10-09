#pragma once


namespace nile {

  class Texture2D;

  struct SpriteComponent {
    // TODO(stel): fix this, make texture as a handle
    Texture2D *texture;
  };

}    // namespace nile
