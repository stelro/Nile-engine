/* ================================================================================
$File: settings.cc
$Date: $
$Revision: $
$Creator: Rostislav Orestis Stelmach
$Notice: $
================================================================================ */

#include "Nile/core/settings.hh"

namespace nile {

  Settings::Settings( u32 width, u32 height, u32 flags, bool debugMode, const std::string &title,
                      ProgramMode programMode ) noexcept
      : m_width( width )
      , m_height( height )
      , m_windowFlags( flags )
      , m_debugMode( debugMode )
      , m_windowTitle( title )
      , m_programMode( programMode ) {}

  Settings::~Settings() noexcept {}

}    // namespace nile
