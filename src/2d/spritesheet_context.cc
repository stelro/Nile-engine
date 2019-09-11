#include "2d/spritesheet_context.hh"

namespace nile {

  SpriteSheetContext::SpriteSheetContext() noexcept {
    // Empty Constructor
  }

  SpriteSheetContext::~SpriteSheetContext() noexcept {
    // Empty Destructor
  }

  void SpriteSheetContext::playAnimation( const std::string &name, const glm::vec2 &position,
                                          u32 speed ) noexcept {
    auto it = m_spriteSheets.find( name );
    if ( it != m_spriteSheets.end() ) {
      it->second->playAnimation( position, speed );
    } else {
      log::error( "%s Sprite Sheet not found in SpriteSheetContext\n" );
    }
  }

  void SpriteSheetContext::playAnimationAndHalt( const std::string &name, const glm::vec2 &position,
                                                 u32 speed ) noexcept {
    auto it = m_spriteSheets.find( name );
    if ( it != m_spriteSheets.end() ) {
      it->second->playAnimationAndHalt( position, speed );
    } else {
      log::error( "%s Sprite Sheet not found in SpriteSheetContext\n" );
    }
  }

  std::shared_ptr<SpriteSheet> SpriteSheetContext::getSpriteSheet( const std::string &name ) const
      noexcept {
    auto it = m_spriteSheets.find( name );
    if ( it != m_spriteSheets.end() ) {
      return it->second;
    } else {
      return nullptr;
    }
  }

}    // namespace nile