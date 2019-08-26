#pragma once

namespace nile {

  class BaseRenderer {
  public:
    virtual ~BaseRenderer() noexcept;
    virtual void initRenderer() noexcept = 0;
    virtual void initWindow() noexcept = 0;
    virtual void run() noexcept = 0;
    virtual void cleanUp() noexcept = 0;
    virtual void mainLoop() noexcept = 0;
  };

}    // namespace nile
