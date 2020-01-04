#pragma once

#include <memory>

namespace nile {

  class Settings;

  class RenderingDevice {
  protected:
    std::shared_ptr<Settings> m_settings;

  public:
    RenderingDevice( const std::shared_ptr<Settings> &settings ) noexcept;
    virtual ~RenderingDevice() noexcept {}
    virtual void initialize() noexcept = 0;
    virtual void destory() noexcept = 0;
    
    virtual void submitFrame() noexcept = 0;
    virtual void endFrame() noexcept = 0;
  };

}    // namespace nile
