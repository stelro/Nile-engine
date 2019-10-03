#include "enviroment_items.hh"
#include <Nile/renderer/shaderset.hh>
#include <Nile/renderer/texture2d.hh>

namespace platformer {

  using namespace nile;

  EnviromentItems::EnviromentItems( const std::shared_ptr<nile::Settings> &settings,
                                    const std::shared_ptr<nile::AssetManager> &manager ) noexcept
      : m_settings( settings )
      , m_assetManager( manager ) {}

  void EnviromentItems::initialize() noexcept {

    auto torchTexture = m_assetManager->loadAsset<Texture2D>(
        "torch_flame", "../assets/textures/torch/flames.png" );
    
    auto torchHandleTex = m_assetManager->loadAsset<Texture2D>(
        "torch_handle", "../assets/textures/torch/torch_handle.png");
    

    auto spriteSheetShader = m_assetManager->getAsset<ShaderSet>( "spritesheet_shader" );

    m_torchSpriteSheet =
        std::make_unique<SpriteSheet>( spriteSheetShader, torchTexture, glm::vec2( 16, 16 ) );
    m_torchSpriteSheet->scale(2.8f);
  
  }

  void EnviromentItems::update( f32 deltaTime ) noexcept {}

  void EnviromentItems::draw( f32 deltaTime ) noexcept {

    m_torchSpriteSheet->playAnimation( glm::vec2( 40, 40 ), 80 );
  }


}    // namespace platformer
