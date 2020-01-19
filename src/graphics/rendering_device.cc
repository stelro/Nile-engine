#include "Nile/graphics/rendering_device.hh"

namespace nile {

  RenderingDevice::RenderingDevice( const std::shared_ptr<Settings> &settings ) noexcept
      : m_settings( settings ) {}

}    // namespace nile
