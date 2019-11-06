#include "Nile/editor/editor.hh"

#include "Nile/core/file_system.hh"
#include "Nile/editor/info_overlay_struct.hh"
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

    // Keeps all the information about the engine in real-time
    // and then show them up to Overlay InfoWindow
    InfoOverlayStruct *m_overlayStruct;

    InfoWindow *m_infoWindow;
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
        , m_context {context}
        , m_overlayStruct( new InfoOverlayStruct() )
        , m_infoWindow( new InfoWindow( m_overlayStruct ) ) {

      this->setup();
    }

    // EditorImpl( EditorImpl &&rhs ) noexcept = default;
    // EditorImpl &operator=( EditorImpl &&rhs ) noexcept = default;

    ~EditorImpl() noexcept;

    void render( float dt ) noexcept;
    void update( float dt ) noexcept;

    void setFps( f32 fps ) noexcept {
      m_overlayStruct->fps = fps;
    }

    void setEntities( u32 entities ) noexcept {
      m_overlayStruct->entities = entities;
    }

    void setComponents( u32 components ) noexcept {
      m_overlayStruct->components = components;
    }

    void setEcsSystems( u32 systems ) noexcept {
      m_overlayStruct->ecs_systems = systems;
    }

    void setUptime(u32 uptime) noexcept {
      m_overlayStruct->uptime = uptime;
    }

    void setLoadersCount(u32 loaders) noexcept {
      m_overlayStruct->asset_loaders = loaders;
    }
  };

  Editor::EditorImpl::~EditorImpl() noexcept {

    delete m_infoWindow;
    delete m_overlayStruct;

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

    io.Fonts->AddFontFromFileTTF(
        FileSystem::getPath( "assets/fonts/LiberationMono-Regular.ttf" ).c_str(), 18.0f, nullptr,
        io.Fonts->GetGlyphRangesDefault() );

    ImGui::StyleColorsDark();

    // Setup platform/Renderer bindigns
    ImGui_ImplSDL2_InitForOpenGL( m_window, m_context );
    ImGui_ImplOpenGL3_Init( "#version 150" );

    // @fix(stel): maybe move the connection with streamlogger to x_host* class?
    log::on_message.connect( &m_consoleLog, &ConsoleLog::appendLogToBuffer );
    log::on_message.connect(
        []( const char *msg, LogType type ) { StreamLogger::printToStream( msg, type ); } );
  }

  void Editor::EditorImpl::render( float dt ) noexcept {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL2_NewFrame( m_window );
    ImGui::NewFrame();

    if ( m_showDemo )
      ImGui::ShowDemoWindow( &m_showDemo );

    if ( m_showInfoWindow )
      m_infoWindow->render( dt );
    if ( m_showConsoleLog )
      m_consoleLog.render( dt );

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData( ImGui::GetDrawData() );
  }

  void Editor::EditorImpl::update( float dt ) noexcept {

    if ( m_showInfoWindow )
      m_infoWindow->update( dt );
    if ( m_showConsoleLog )
      m_consoleLog.update( dt );

    m_showInfoWindow = m_infoWindow->isInfoWindowOpen();
    m_showConsoleLog = m_consoleLog.logConsoleIsOpen();
  }

  // Editor
  Editor::Editor( SDL_Window *window, SDL_GLContext context ) noexcept
      : m_impl {std::make_unique<EditorImpl>( window, context )} {}

  void Editor::render( f32 dt ) noexcept {
    m_impl->render( dt );
  }

  void Editor::update( f32 dt ) noexcept {
    m_impl->update( dt );
  }

  void Editor::setFps( f32 fps ) noexcept {
    m_impl->setFps( fps );
  }

  void Editor::setEntities( u32 entities ) noexcept {
    m_impl->setEntities( entities );
  }

  void Editor::setComponents( u32 components ) noexcept {
    m_impl->setComponents( components );
  }

  void Editor::setEcsSystems( u32 systems ) noexcept {
    m_impl->setEcsSystems( systems );
  }

  void Editor::setUptime(u32 uptime) noexcept {
    m_impl->setUptime(uptime);
  }

  void Editor::setLoadersCount(u32 loaders) noexcept {
    m_impl->setLoadersCount(loaders);
  }

  Editor::~Editor() noexcept = default;
  Editor::Editor( Editor &&rhs ) noexcept = default;
  Editor &Editor::operator=( Editor &&rhs ) noexcept = default;

}    // namespace nile::editor
