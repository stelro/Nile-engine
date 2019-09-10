#pragma once

#include "2d/sprite_sheet.hh"

#include <memory>
#include <string>
#include <unordered_map>


namespace nile {

  class SpriteSheetContext {
  private:
    std::unordered_map<std::string, std::shared_ptr<SpriteSheet>> m_spriteSheets;

  public:
    SpriteSheetContext() noexcept;
    ~SpriteSheetContext() noexcept;

    template <typename... Args>
    void addSpriteSheet( const std::string &name, Args &&... args ) noexcept;

    // This is regular animation, which "plays" all the frams of the sprite
    void playAnimation( const std::string &name, const glm::vec2 &position,
                        u32 speed = 60 ) noexcept;

    // playAnimationAndHalt is basically playing all the frames of the sprite
    // and halts until all the frames are ended
    void playAnimationAndHalt( const std::string &name, const glm::vec2 &position,
                        u32 speed = 60 ) noexcept;


    std::shared_ptr<SpriteSheet> getSpriteSheet(const std::string& name) const noexcept;
  };

  template <typename... Args>
  void SpriteSheetContext::addSpriteSheet( const std::string &name, Args &&... args ) noexcept {
    m_spriteSheets[ name ] = std::make_shared<SpriteSheet>( std::forward<Args>( args )... );
  }

}    // namespace nile
