#pragma once

#include "Nile/core/types.hh"

namespace nile {

  struct Relationship {
    usize size;

    Entity parent {ecs::null};
    Entity firstChild {ecs::null};

    Entity next {ecs::null};
    Entity prev {ecs::null};
  };

}    // namespace nile
