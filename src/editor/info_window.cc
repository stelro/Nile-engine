#include "Nile/editor/info_window.hh"
#include "Nile/utils/string_utils.hh"

#include "imgui/imgui.h"

namespace nile::editor {

  InfoWindow::InfoWindow() noexcept {
    m_labels.reserve( MAX_LABELS_SIZE );
  }

  void InfoWindow::render( float dt ) noexcept {

    const float DISTANCE = 10.0f;
    static int corner = 0;
    ImGuiIO &io = ImGui::GetIO();
    if ( corner != -1 ) {
      ImVec2 window_pos = ImVec2( ( corner & 1 ) ? io.DisplaySize.x - DISTANCE : DISTANCE,
                                  ( corner & 2 ) ? io.DisplaySize.y - DISTANCE : DISTANCE );
      ImVec2 window_pos_pivot =
          ImVec2( ( corner & 1 ) ? 1.0f : 0.0f, ( corner & 2 ) ? 1.0f : 0.0f );
      ImGui::SetNextWindowPos( window_pos, ImGuiCond_Always, window_pos_pivot );
    }
    ImGui::SetNextWindowBgAlpha( 0.35f );    // Transparent background
    if ( ImGui::Begin( "Example: Simple overlay", &m_infoWindowIsOpen,
                       ( corner != -1 ? ImGuiWindowFlags_NoMove : 0 ) |
                           ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize |
                           ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing |
                           ImGuiWindowFlags_NoNav ) ) {
      ImGui::Text( "Simple overlay\n"
                   "in the corner of the screen.\n"
                   "(right-click to change position)" );

      for ( const auto &i : m_labels ) {
        ImGui::Text( "%s", i.c_str() );
      }

      ImGui::Separator();
      if ( ImGui::IsMousePosValid() )
        ImGui::Text( "Mouse Position: (%.1f,%.1f)", io.MousePos.x, io.MousePos.y );
      else
        ImGui::Text( "Mouse Position: <invalid>" );
      if ( ImGui::BeginPopupContextWindow() ) {
        if ( ImGui::MenuItem( "Custom", NULL, corner == -1 ) )
          corner = -1;
        if ( ImGui::MenuItem( "Top-left", NULL, corner == 0 ) )
          corner = 0;
        if ( ImGui::MenuItem( "Top-right", NULL, corner == 1 ) )
          corner = 1;
        if ( ImGui::MenuItem( "Bottom-left", NULL, corner == 2 ) )
          corner = 2;
        if ( ImGui::MenuItem( "Bottom-right", NULL, corner == 3 ) )
          corner = 3;
        if ( m_infoWindowIsOpen && ImGui::MenuItem( "Close" ) )
          m_infoWindowIsOpen = false;
        ImGui::EndPopup();
      }
    }
    ImGui::End();
  }

  void InfoWindow::update( float dt ) noexcept {}

  void InfoWindow::addLabel( const char *fmt, ... ) noexcept {
    va_list args;
    va_start( args, fmt );
    m_labels.emplace_back( VaArgsToString( fmt, args ) );
    m_labelsSize++;
    va_end( args );
  }

}    // namespace nile::editor
