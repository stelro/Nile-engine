#include "enviroment_items.hh"
#include <Nile/2d/sprite.hh>
#include <Nile/asset/builder/shaderset_builder.hh>
#include <Nile/renderer/shaderset.hh>
#include <Nile/renderer/texture2d.hh>

namespace platformer {

  using namespace nile;

  EnviromentItems::EnviromentItems( const std::shared_ptr<nile::Settings> &settings,
                                    const std::shared_ptr<nile::AssetManager> &manager,
                                    const std::shared_ptr<nile::SpriteRenderer> &renderer,
                                    const std::shared_ptr<Camera2D> &camera ) noexcept
      : m_settings( settings )
      , m_assetManager( manager )
      , m_spriteRenderer( renderer )
      , m_camera( camera ) {}

  void EnviromentItems::initialize() noexcept {

    auto spriteSheetShader = m_assetManager->createBuilder<ShaderSet>()
                                 .setVertexPath( "../assets/shaders/spritesheet_vertex.glsl" )
                                 .setFragmentPath( "../assets/shaders/spritesheet_fragment.glsl" )
                                 .build();


    m_assetManager->storeAsset<ShaderSet>( "envitems_shader", spriteSheetShader );


    auto torchTexture = m_assetManager->loadAsset<Texture2D>(
        "torch_flame", "../assets/textures/torch/flames.png" );

    m_staticSprites.emplace_back( Sprite( m_assetManager->loadAsset<Texture2D>(
        "torch_handle", "../assets/textures/torch/torch_handle.png" ) ) );
    m_staticSprites.back().setPosition( glm::vec2( 200, 200 ) );
    m_staticSprites.back().setSize( glm::vec2( 13.2f ) );
    m_staticSprites.back().setColor( glm::vec3( 1.0f ) );



    m_torchSpriteSheet =
        std::make_unique<SpriteSheet>( spriteSheetShader, torchTexture, glm::vec2( 16, 16 ) );
    m_torchSpriteSheet->scale( 1.0f );
  }

  void EnviromentItems::update( f32 deltaTime ) noexcept {}

  void EnviromentItems::draw( f32 deltaTime ) noexcept {

    for ( const auto &i : m_staticSprites ) {
      m_spriteRenderer->draw( i );
    }

    m_torchSpriteSheet->playAnimation( glm::vec2( 200, 190 ), 80 );

    m_assetManager->getAsset<ShaderSet>( "envitems_shader" )
        ->use()
        .SetMatrix4( "projection", m_camera->getCameraMatrix() );
  }


}    // namespace platformer
