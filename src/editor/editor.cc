#include "Nile/editor/editor.hh"

#include "Nile/editor/info_window.hh"
#include "Nile/log/editor_console_logger.hh"
#include "Nile/log/log.hh"
#include "Nile/log/stream_logger.hh"

#include "imgui/imgui.h"
#include "imgui/imgui_impl_opengl3.h"
#include "imgui/imgui_impl_sdl.h"

#include <functional>

namespace nile::editor {

  class Editor::EditorImpl {
  private:
    SDL_Window *m_window;
    SDL_GLContext m_context;

    InfoWindow m_infoWindow;
    ConsoleLog m_consoleLog;

    bool m_showEditor = true;
    bool m_showDemo = false;
    bool m_showInfoWindow = true;
    bool m_showDebugConsole = false;
    bool m_showConsoleLog = true;

    void setup() noexcept;

  public:
    EditorImpl( SDL_Window *window, SDL_GLContext context ) noexcept
        : m_window {window}
        , m_context {context} {
      this->setup();
    }

    // EditorImpl( EditorImpl &&rhs ) noexcept = default;
    // EditorImpl &operator=( EditorImpl &&rhs ) noexcept = default;

    ~EditorImpl() noexcept;

    void render( float dt ) noexcept;
    void update( float dt ) noexcept;
  };

  Editor::EditorImpl::~EditorImpl() noexcept {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();
  }

  void Editor::EditorImpl::setup() noexcept {

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    ImGuiIO &io = ImGui::GetIO();
    ( void )io;

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();

    // Setup platform/Renderer bindigns
    ImGui_ImplSDL2_InitForOpenGL( m_window, m_context );
    ImGui_ImplOpenGL3_Init( "#version 150" );


    log::on_message.connect( &m_consoleLog, &ConsoleLog::appendLogToBuffer );
    log::on_message.connect(
        []( const char *msg, LogType type ) { StreamLogger::printToStream( msg, type ); } );

    log::error( "This is error: \n" );
    // log::warning( "This is warning: \n" );
    // log::fatal( "This is fatal: \n" );
    // log::notice( "This is notice: \n" );
    // log::print( "This is print: \n" );
    // log::console( "This is console: \n" );
  }

  void Editor::EditorImpl::render( float dt ) noexcept {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL2_NewFrame( m_window );
    ImGui::NewFrame();

    if ( m_showDemo )
      ImGui::ShowDemoWindow( &m_showDemo );

    if ( m_showInfoWindow )
      m_infoWindow.render( dt );
    if ( m_showConsoleLog )
      m_consoleLog.render( dt );

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData( ImGui::GetDrawData() );
  }

  void Editor::EditorImpl::update( float dt ) noexcept {

    if ( m_showInfoWindow )
      m_infoWindow.update( dt );
    if ( m_showConsoleLog )
      m_consoleLog.update( dt );

    m_showInfoWindow = m_infoWindow.isInfoWindowOpen();
    m_showConsoleLog = m_consoleLog.logConsoleIsOpen();
  }

  // Editor
  Editor::Editor( SDL_Window *window, SDL_GLContext context ) noexcept
      : m_impl {std::make_unique<EditorImpl>( window, context )} {}

  void Editor::render( float dt ) noexcept {
    m_impl->render( dt );
  }

  void Editor::update( float dt ) noexcept {
    m_impl->update( dt );
  }

  Editor::~Editor() noexcept = default;
  Editor::Editor( Editor &&rhs ) noexcept = default;
  Editor &Editor::operator=( Editor &&rhs ) noexcept = default;

}    // namespace nile::editor