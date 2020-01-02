#pragma once

#include <memory>

namespace nile {

  class Texture2D;

  struct SpriteComponent {
    SpriteComponent()
        : texture( nullptr ) {}
    SpriteComponent( std::shared_ptr<Texture2D> &tex )
        : texture( tex ) {}
    std::shared_ptr<Texture2D> texture;
  };

}    // namespace nile
