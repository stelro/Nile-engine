#pragma once

#include "Nile/core/assert.hh"

#include <vulkan/vulkan.h>

#include <string>

namespace nile {

#define VK_CHECK_RESULT( f )                                                                       \
  {                                                                                                \
    VkResult res = ( f );                                                                          \
    std::string error {"VkResult Failed with error: " + vulkanErrToString( res )};                 \
    ASSERT_M( res == VK_SUCCESS, error.c_str() );                                                             \
  }

  std::string vulkanErrToString( VkResult error ) noexcept;

  bool isVkExtensionSupported(const char* extension) noexcept;

}    // namespace nile
