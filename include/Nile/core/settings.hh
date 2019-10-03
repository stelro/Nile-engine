/* ================================================================================
$File: settings.hh
$Date: $
$Revision: $
$Creator: Rostislav Orestis Stelmach
$Notice: $
================================================================================ */

#pragma once

#include "Nile/core/nile.hh"
#include <string>

namespace nile {

  class Settings {
  private:
    // Screen width and height
    u32 m_width;
    u32 m_height;
    u32 m_windowFlags;
    bool m_debugMode;
    // Window title
    std::string m_windowTitle;

  public:
    class Builder;

    Settings( u32 width, u32 height, u32 windowFlags,bool m_debugMode,
              const std::string &title ) noexcept;
    ~Settings() noexcept;

    [[nodiscard]] inline u32 getWidth() const noexcept {
      return m_width;
    }

    [[nodiscard]] inline u32 getHeight() const noexcept {
      return m_height;
    }

    [[nodiscard]] inline std::string getWindowTitle() const noexcept {
      return m_windowTitle;
    }

    [[nodiscard]] inline u32 getWindowFlags() const noexcept {
      return m_windowFlags;
    }

    [[nodiscard]] inline bool getDebugMode() const noexcept {
      return m_debugMode;
    }
  };

  class Settings::Builder {
  private:
    u32 m_width = 0;
    u32 m_height = 0;
    u32 m_windowFlags = 0x0;
    bool m_debugMode = false;
    std::string m_windowTitle {};

  public:
    Builder &setWidth( u32 width ) noexcept {
      this->m_width = width;
      return *this;
    }

    Builder &setHeight( u32 height ) noexcept {
      this->m_height = height;
      return *this;
    }

    Builder &setTitle( const std::string &name ) noexcept {
      this->m_windowTitle = name;
      return *this;
    }

    Builder &setWindowFlags( u32 flags ) noexcept {
      this->m_windowFlags = flags;
      return *this;
    }

    Builder &setDebugMode( bool flag ) noexcept {
      this->m_debugMode = flag;
      return *this;
    }

    Settings build() const noexcept {
      return Settings( m_width, m_height, m_windowFlags, m_debugMode, m_windowTitle );
    }
  };

}    // namespace nile
