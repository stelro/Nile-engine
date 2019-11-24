/* ================================================================================
$File: sprite_set.cc
$Date: 03 Oct 2019
$Revision: $
$Creator: Rostislav Orestis Stelmach
$Notice: $
================================================================================ */

#include "Nile/2d/sprite_set.hh"
#include "Nile/2d/sprite.hh"
#include "Nile/2d/sprite_sheet.hh"
#include <vector>
namespace nile {

  SpriteSet::SpriteSet() noexcept {}

  void SpriteSet::addSprite( const Sprite &sprite ) noexcept {
    m_sprites.emplace_back( std::make_unique<Sprite>( sprite ) );
  }

  void SpriteSet::addSprite( Sprite &&sprite ) noexcept {
    m_sprites.emplace_back( std::make_unique<Sprite>( std::move( sprite ) ) );
  }

  void SpriteSet::addSpriteSheet( const SpriteSheet &spritesheet ) noexcept {
    m_spriteSheets.emplace_back( std::make_unique<SpriteSheet>( spritesheet ) );
  }

  void SpriteSet::addSpriteSheet( SpriteSheet &&spritesheet ) noexcept {
    m_spriteSheets.emplace_back( std::make_unique<SpriteSheet>( std::move( spritesheet ) ) );
  }

}    // namespace nile
