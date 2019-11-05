#include "Nile/log/editor_console_logger.hh"
#include <iostream>

namespace nile::editor {

  ConsoleLog::ConsoleLog( bool autoScroll ) noexcept
      : m_autoScroll( autoScroll )
      , m_title( "Console Log" ) {
    this->clear();
  }

  void ConsoleLog::clear() noexcept {
    m_buffer.clear();
    m_linesOffset.clear();
    m_linesOffset.push_back( 0 );
  }

  void ConsoleLog::appendLogToBuffer( const char *fmt, LogType type ) noexcept {

    auto old_size = m_buffer.size();
    m_buffer.append( fmt );
    for ( int new_size = m_buffer.size(); old_size < new_size; old_size++ )
      if ( m_buffer[ old_size ] == '\n' )
        m_linesOffset.push_back( old_size + 1 );
  }


  void ConsoleLog::render( float dt ) noexcept {

    ImGui::SetNextWindowSize( ImVec2( 100, 400 ), ImGuiCond_FirstUseEver );
    if ( !ImGui::Begin( m_title.c_str(), &m_logConsoleIsOpen ) ) {
      ImGui::End();
      return;
    }

    // Options menu
    if ( ImGui::BeginPopup( "Options" ) ) {
      ImGui::Checkbox( "Auto-scroll", &m_autoScroll );
      ImGui::EndPopup();
    }

    // Main window
    if ( ImGui::Button( "Options" ) )
      ImGui::OpenPopup( "Options" );
    ImGui::SameLine();
    bool clear = ImGui::Button( "Clear" );
    ImGui::SameLine();
    bool copy = ImGui::Button( "Copy" );
    ImGui::SameLine();
    m_filter.Draw( "Filter", -100.0f );

    ImGui::Separator();
    ImGui::BeginChild( "scrolling", ImVec2( 0, 0 ), false, ImGuiWindowFlags_HorizontalScrollbar );

    if ( clear )
      this->clear();
    if ( copy )
      ImGui::LogToClipboard();

    ImGui::PushStyleVar( ImGuiStyleVar_ItemSpacing, ImVec2( 0, 0 ) );
    const char *buf = m_buffer.begin();
    const char *buf_end = m_buffer.end();

    if ( m_filter.IsActive() ) {
      // for ( int line_no = 0; line_no < m_linesOffset.Size; line_no++ ) {
      for ( int line_no = 0; line_no < m_linesOffset.Size; line_no++ ) {
        const char *line_start = buf + m_linesOffset[ line_no ];
        const char *line_end = ( line_no + 1 < m_linesOffset.Size )
                                   ? ( buf + m_linesOffset[ line_no + 1 ] - 1 )
                                   : buf_end;

        if ( m_filter.PassFilter( line_start, line_end ) )
          ImGui::TextUnformatted( line_start, line_end );
      }
    } else {
      // The simplest and easy way to display the entire buffer:
      //   ImGui::TextUnformatted(buf_begin, buf_end);
      // And it'll just work. TextUnformatted() has specialization for large blob of text and will
      // fast-forward to skip non-visible lines. Here we instead demonstrate using the clipper to
      // only process lines that are within the visible area. If you have tens of thousands of
      // items and their processing cost is non-negligible, coarse clipping them on your side is
      // recommended. Using ImGuiListClipper requires A) random access into your data, and B)
      // items all being the  same height, both of which we can handle since we an array pointing
      // to the beginning of each line of text. When using the filter (in the block of code above)
      // we don't have random access into the data to display anymore, which is why we don't use
      // the clipper. Storing or skimming through the search result would make it possible (and
      // would be recommended if you want to search through tens of thousands of entries)
      ImGuiListClipper clipper;
      clipper.Begin( m_linesOffset.Size );
      while ( clipper.Step() ) {
        for ( int line_no = clipper.DisplayStart; line_no < clipper.DisplayEnd; line_no++ ) {

          const char *line_start = buf + m_linesOffset[ line_no ];
          const char *line_end = ( line_no + 1 < m_linesOffset.Size )
                                     ? ( buf + m_linesOffset[ line_no + 1 ] - 1 )
                                     : buf_end;


          ImGui::TextUnformatted( line_start, line_end );
        }
      }
      clipper.End();
    }
    ImGui::PopStyleVar();

    if ( m_autoScroll && ImGui::GetScrollY() >= ImGui::GetScrollMaxY() )
      ImGui::SetScrollHereY( 1.0f );

    ImGui::EndChild();
    ImGui::End();
  }    // namespace nile::editor

  void ConsoleLog::update( float dt ) noexcept {}

}    // namespace nile::editor
