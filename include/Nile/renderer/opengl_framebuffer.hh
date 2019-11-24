#pragma once

#include "Nile/core/types.hh"

#include <memory>

namespace nile {

  class Settings;

  class OpenglFramebuffer {
  private:
    std::shared_ptr<Settings> m_settings;
    u32 m_fbo;
    u32 m_textureColorBuffer;
    
    // renderbuffer object
    u32 m_rbo;

    u32 m_quadVao;
    u32 m_quadVbo;

    bool m_readyToRender = false;
    void initialize() noexcept;

  public:
    OpenglFramebuffer( const std::shared_ptr<Settings> &settings ) noexcept;
    ~OpenglFramebuffer() noexcept;

    void bind() noexcept;
    void unbind() noexcept;

    // We initialize and prepare a quad to render the contents from the
    // framebuffer to the quad in the screen.
    // This method should be called only once, and after thet in the gameloop
    // submitFrame() is expected
    void prepareQuad() noexcept;

    // This is used to submit the rendered framebuffer to the screen
    // e.g in a quad. This method is NOT used if we desire off-screen rendering
    void submitFrame() noexcept;

    void bindCollorBuffer() noexcept;
  };

}    // namespace nile
