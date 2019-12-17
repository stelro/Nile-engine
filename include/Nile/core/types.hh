/* ================================================================================
$File: types.hh
$Date: 09 Oct 2019 $
$Revision: $
$Creator: Rostislav Orestis Stelmach
$Notice: $
================================================================================ */

#pragma once

#include <bitset>
#include <cstdint>

namespace nile {

  // Types aliases
  using i8 = int8_t;
  using i16 = int16_t;
  using i32 = int32_t;
  using i64 = int64_t;
  using u8 = uint8_t;
  using u16 = uint16_t;
  using u32 = uint32_t;
  using u64 = uint32_t;
  using f32 = float;
  using f64 = double;
  using usize = std::size_t;
  using uptr = uintptr_t;


  // ECS
  using Entity = std::uint32_t;
  using ComponentType = std::uint8_t;


  namespace ecs {
    constexpr Entity MAX_ENTITIES = 32768;
    constexpr ComponentType MAX_COMPONENTS = 64;
    constexpr Entity null = 0;
  }    // namespace ecs

  using Signature = std::bitset<ecs::MAX_COMPONENTS>;

}    // namespace nile
