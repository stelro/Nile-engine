#include "text_buffer.hh"

namespace platformer {

  using namespace nile;

  TextBuffer::TextBuffer( std::shared_ptr<Settings> &settings,
                          std::shared_ptr<Coordinator> &coordinator,
                          const std::shared_ptr<AssetManager> &assetManager ) noexcept
      : m_settings( settings )
      , m_ecsCoordinator( coordinator )
      , m_assetManager( assetManager ) {}

  void TextBuffer::append( const std::string &text, const glm::vec2 &position,
                           const glm::vec3 col ) noexcept {

    BufferElement element;
    element.text = std::make_unique<ScreenText>( m_settings, m_ecsCoordinator, m_assetManager );
    element.timer.start();
    element.text->print( text, position, col );
    m_elementsStack.push( std::move( element ) );
  }

  void TextBuffer::update( f32 deltaTime ) noexcept {


    for ( size_t i = 0; i < m_elementsStack.size(); i++ ) {
      //log::print("element %d\n", m_elementsStack.size());

      if ( m_elementsStack.top().timer.getTicks() >= 4000 )
        m_elementsStack.pop();
    }
  }

}    // namespace platformer
