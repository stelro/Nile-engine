/* ================================================================================
$File: sprite_set.hh
$Date: 03 Oct 2019
$Revision: $
$Creator: Rostislav Orestis Stelmach
$Notice: $
================================================================================ */

#pragma once

// @brief:
// SpriteSet is a renderable combination set of static sprites and animated sprites
// (spritesheets), and used for porpuses where we want to combnine one or more animated
// spritesheet with one or more static sprite
// example, if we have a fire-torch, and fire is animated ( by spritesheet ) but the handle
// of the torch is static, thus we can have a spriteset as a whole, and place it in the world
// chaning the positon as a whole set or speretally

#include "Nile/2d/sprite_renderer.hh"

#include <memory>
#include <vector>

namespace nile {

  class Sprite;
  class SpriteSheet;

  class SpriteSet {
  private:
    std::vector<std::unique_ptr<Sprite>> m_sprites;
    std::vector<std::unique_ptr<SpriteSheet>> m_spriteSheets;

  public:
    SpriteSet() noexcept;

    void addSprite( const Sprite &sprite ) noexcept;
    void addSprite( Sprite &&sprite ) noexcept;

    void addSpriteSheet( const SpriteSheet &spritesheet ) noexcept;
    void addSpriteSheet( SpriteSheet &&spritesheet ) noexcept;

    void draw(const SpriteRenderer& spriteRenderer) noexcept;
  };

}    // namespace nile
