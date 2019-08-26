#include "core/settings.hh"

namespace nile {

  Settings::Settings( u32 width, u32 height, u32 flags, const std::string &title ) noexcept
      : m_width( width )
      , m_height( height )
      , m_windowFlags( flags )
      , m_windowTitle( title ) {}

}    // namespace nile
