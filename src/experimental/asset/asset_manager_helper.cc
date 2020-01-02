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

    while ( true ) {

    log::print( "reload shaders\n" );

    auto shader = m_assetManager->getAsset<ShaderSet>( "model_shader" );


    // auto shader = m_assetManager->getAsset<ShaderSet>( "model_shader" );
    //
    // auto modelShader = m_assetManager->createBuilder<ShaderSet>()
    //                        .setVertexPath( "../assets/shaders/model.vert.glsl" )
    //                        .setFragmentPath( "../assets/shaders/model.frag.glsl" )
    //                        .build();
    //    shader->resetAsset();
    //   shader->setProgramId( modelShader->getProgramId() );

    std::this_thread::sleep_for( std::chrono::seconds( 1 ) );
    }

    //    m_assetManager->replace("model_shader", modelShader);

    // for (    auto& [name,asset] : *m_assetManager ) {
    //   log::print("reloading asset: %s\n", name.c_str());
    //   auto prev_asset = m_assetManager->getAsset<decltype(asset)>(name);
    //
    // }
  }


}    // namespace nile
