#pragma once

#include "Nile/core/types.hh"
#include "Nile/log/log_type.hh"
#include "imgui/imgui.h"

#include <string>

namespace nile::editor {


  class ConsoleLog {
  private:
    ImGuiTextBuffer m_buffer;
    ImGuiTextFilter m_filter;
    ImVector<i32> m_linesOffset;

    bool m_autoScroll;
    bool m_logConsoleIsOpen = true;

    std::string m_title {};


  public:
    explicit ConsoleLog( bool autoScroll = true ) noexcept;
    void render( float dt ) noexcept;
    void update( float dt ) noexcept;

    void appendLogToBuffer( const char *fmt, LogType type ) noexcept;

    void clear() noexcept;

    void setLogConsleIsOpen( bool value ) noexcept {
      m_logConsoleIsOpen = value;
    }

    bool logConsoleIsOpen() const noexcept {
      return m_logConsoleIsOpen;
    }
  };

}    // namespace nile::editor
