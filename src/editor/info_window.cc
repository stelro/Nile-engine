#include "Nile/editor/info_window.hh"
#include "Nile/utils/string_utils.hh"

#include "imgui/imgui.h"

namespace nile::editor {

  InfoWindow::InfoWindow( InfoOverlayStruct *ov_struct ) noexcept
      : m_overlayStruct( ov_struct ) {
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
    ImGui::SetNextWindowBgAlpha( 0.25f );    // Transparent background
    if ( ImGui::Begin( "Info Overlay", &m_infoWindowIsOpen,
                       ( corner != -1 ? ImGuiWindowFlags_NoMove : 0 ) |
                           ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize |
                           ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav ) ) {

      this->printStruct();

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

  void InfoWindow::printStruct() const noexcept {

    ImGui::PushStyleColor( ImGuiCol_Text, ImVec4( 0.678f, 0.925f, 0.412f, 1.0f ) );
    ImGui::Text( "FPS: %.2f ( %.3f ms)\n", ( 1000.f / m_overlayStruct->fps ),
                 m_overlayStruct->fps );
    ImGui::Text( "Uptime: %us\n", m_overlayStruct->uptime / 1000);
    ImGui::Text( "Vertices: %u\n", m_overlayStruct->vertices );
    ImGui::Text( "Uniform loads: %u\n", m_overlayStruct->uniform_loads );
    ImGui::Text( "Asset loaders: %u\n", m_overlayStruct->asset_loaders );
    ImGui::Separator();
    ImGui::Text( "Entities: %u\n", m_overlayStruct->entities );
    ImGui::Text( "Components: %u\n", m_overlayStruct->components );
    ImGui::Text( "Systems: %u\n", m_overlayStruct->ecs_systems );
    ImGui::Spacing();
    ImGui::Spacing();
    ImGui::PopStyleColor();
  }

}    // namespace nile::editor
