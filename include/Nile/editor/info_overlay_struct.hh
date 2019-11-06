#pragma once

#include "Nile/core/types.hh"

namespace nile {

  struct InfoOverlayStruct {

    f32 fps = 0;
    u32 uptime = 0;
    u32 vertices = 0;
    u32 uniform_loads = 0;
    u32 asset_loaders = 0;
    u32 textures = 0;
    u32 textures_mb = 0;

    u32 entities = 0;
    u32 components = 0;
    u32 ecs_systems = 0;
  };

}    // namespace nile
