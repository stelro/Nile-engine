#include "Nile/experimental/asset/asset_manager_helper.hh"

#include "Nile/asset/asset_manager.hh"
#include "Nile/asset/builder/shaderset_builder.hh"
#include "Nile/log/log.hh"
#include "Nile/renderer/shaderset.hh"

#include <chrono>
#include <thread>

namespace nile {

  AssetManagerHelper::AssetManagerHelper(
      const std::shared_ptr<AssetManager> &assetManager ) noexcept
      : m_assetManager( assetManager ) {}


  void AssetManagerHelper::operator()() noexcept {
    log::print( "reload shaders\n" );

    auto shader = m_assetManager->getAsset<ShaderSet>( "model_shader" );

    auto modelShader = m_assetManager->createBuilder<ShaderSet>()
                           .setVertexPath( "../assets/shaders/model.vert.glsl" )
                           .setFragmentPath( "../assets/shaders/model.frag.glsl" )
                           .build();
    shader->resetAsset();
    shader->setProgramId( modelShader->getProgramId() );

    std::this_thread::sleep_for( std::chrono::seconds( 1 ) );
  }


  void AssetManagerHelper::reloadShaders() noexcept {

    log::print("called reload\n");

    for ( const auto& i : *m_assetManager ) {
      log::print("%s\n", typeid(i.second.get()).name());
    }

    // while ( true ) {
    //
    //   log::print( "reload shaders\n" );
    //
    //
    //   std::this_thread::sleep_for( std::chrono::seconds( 1 ) );
    // }
  }


}    // namespace nile
