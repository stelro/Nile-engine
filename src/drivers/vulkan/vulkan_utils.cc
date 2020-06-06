#include "Nile/drivers/vulkan/vulkan_utils.hh"
#include "Nile/core/types.hh"
#include "Nile/log/log.hh"

#include <cstring>
#include <vector>

namespace nile {

  std::string vulkanErrToString( VkResult error ) noexcept {
    switch ( error ) {
#define VKSTR( r )                                                                                 \
  case VK_##r:                                                                                     \
    return #r
      VKSTR( NOT_READY );
      VKSTR( TIMEOUT );
      VKSTR( EVENT_SET );
      VKSTR( EVENT_RESET );
      VKSTR( INCOMPLETE );
      VKSTR( ERROR_OUT_OF_HOST_MEMORY );
      VKSTR( ERROR_OUT_OF_DEVICE_MEMORY );
      VKSTR( ERROR_INVALID_OPAQUE_CAPTURE_ADDRESS_KHR);
      VKSTR( ERROR_INITIALIZATION_FAILED );
      VKSTR( ERROR_DEVICE_LOST );
      VKSTR( ERROR_MEMORY_MAP_FAILED );
      VKSTR( ERROR_LAYER_NOT_PRESENT );
      VKSTR( ERROR_EXTENSION_NOT_PRESENT );
      VKSTR( ERROR_FEATURE_NOT_PRESENT );
      VKSTR( ERROR_INCOMPATIBLE_DRIVER );
      VKSTR( ERROR_TOO_MANY_OBJECTS );
      VKSTR( ERROR_FORMAT_NOT_SUPPORTED );
      VKSTR( ERROR_SURFACE_LOST_KHR );
      VKSTR( ERROR_NATIVE_WINDOW_IN_USE_KHR );
      VKSTR( SUBOPTIMAL_KHR );
      VKSTR( ERROR_OUT_OF_DATE_KHR );
      VKSTR( ERROR_INCOMPATIBLE_DISPLAY_KHR );
      VKSTR( ERROR_VALIDATION_FAILED_EXT );
      VKSTR( ERROR_INVALID_SHADER_NV );
#undef VKSTR
      default:
        return "UNKNOWN_ERROR";
    }
  }

  bool isVkExtensionSupported( const char *extension ) noexcept {
    u32 extension_count = 0;
    vkEnumerateInstanceExtensionProperties( nullptr, &extension_count, nullptr );
    std::vector<VkExtensionProperties> extensions( extension_count );
    vkEnumerateInstanceExtensionProperties( nullptr, &extension_count, extensions.data() );

    for ( const auto &i : extensions ) {
      if ( strcmp( i.extensionName, extension ) == 0 ) {
        return true;
      }
    }

    return false;
  }

}    // namespace nile
