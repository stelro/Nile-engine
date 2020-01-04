#include "Nile/drivers/vulkan/vulkan_rendering_device.hh"
#include "Nile/core/assert.hh"
#include "Nile/core/settings.hh"
#include "Nile/core/types.hh"
#include "Nile/drivers/vulkan/vulkan_utils.hh"
#include "Nile/log/log.hh"

#include <SDL2/SDL_vulkan.h>

#include <cstring>
#include <set>

namespace nile {

  VkResult CreateDebugUtilsMessengerEXT( VkInstance instance,
                                         const VkDebugUtilsMessengerCreateInfoEXT *pCreateInfo,
                                         const VkAllocationCallbacks *pAllocator,
                                         VkDebugUtilsMessengerEXT *pDebugMessenger ) {
    auto func = ( PFN_vkCreateDebugUtilsMessengerEXT )vkGetInstanceProcAddr(
        instance, "vkCreateDebugUtilsMessengerEXT" );
    if ( func != nullptr ) {
      return func( instance, pCreateInfo, pAllocator, pDebugMessenger );
    } else {
      return VK_ERROR_EXTENSION_NOT_PRESENT;
    }
  }

  void DestroyDebugUtilsMessengerEXT( VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger,
                                      const VkAllocationCallbacks *pAllocator ) {
    auto func = ( PFN_vkDestroyDebugUtilsMessengerEXT )vkGetInstanceProcAddr(
        instance, "vkDestroyDebugUtilsMessengerEXT" );
    if ( func != nullptr ) {
      func( instance, debugMessenger, pAllocator );
    }
  }


  VulkanRenderingDevice::VulkanRenderingDevice( const std::shared_ptr<Settings> &settings ) noexcept
      : RenderingDevice( settings ) {}

  void VulkanRenderingDevice::initialize() noexcept {
    this->initRenderingInternal();
    this->initWindow();
    this->createVulkanInstance();
    this->createSurface();
    this->setupDebugMessenger();
    this->pickPhysicalDevice();
    this->createLogicalDevice();
  }

  void VulkanRenderingDevice::destory() noexcept {

    vkDestroyDevice( m_logicalDevice, nullptr );

    if ( m_enableValidationLayers ) {
      DestroyDebugUtilsMessengerEXT( m_vulkanInstance, m_debugMessenger, nullptr );
    }

    vkDestroySurfaceKHR( m_vulkanInstance, m_surface, nullptr );
    vkDestroyInstance( m_vulkanInstance, nullptr );
    SDL_DestroyWindow( m_window );
    m_window = nullptr;
    SDL_Quit();
  }

  void VulkanRenderingDevice::initRenderingInternal() noexcept {
    // Initialize SDL2 ( used for window creation and keyboard input)
    ASSERT_M( ( SDL_Init( SDL_INIT_VIDEO ) >= 0 ), "Failed to init SDL" )
  }

  void VulkanRenderingDevice::initWindow() noexcept {

    // Create SDL Window
    m_window = SDL_CreateWindow( m_settings->getWindowTitle().c_str(), SDL_WINDOWPOS_CENTERED,
                                 SDL_WINDOWPOS_CENTERED, m_settings->getWidth(),
                                 m_settings->getHeight(), m_settings->getWindowFlags() );
  }

  void VulkanRenderingDevice::createVulkanInstance() noexcept {

    ASSERT_M( m_enableValidationLayers && checkValidationLayerSupport(),
              "Validaion layers requested, but not available!\n" );

    VkApplicationInfo app_info = {};
    app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    app_info.pApplicationName = m_settings->getWindowTitle().c_str();
    app_info.applicationVersion = VK_MAKE_VERSION( 1, 0, 0 );
    app_info.pEngineName = "No Engine";
    app_info.engineVersion = VK_MAKE_VERSION( 1, 0, 0 );
    app_info.apiVersion = VK_API_VERSION_1_0;

    VkInstanceCreateInfo create_info = {};
    create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    create_info.pApplicationInfo = &app_info;

    auto extensions = getRequiredExtensions();

    if ( m_enableValidationLayers ) {
      for ( const auto &extension : extensions )
        log::notice( " <~> enabled vulkan extension: %s\n", extension );
    }

    for ( const auto &extension : extensions )
      ASSERT_M( isVkExtensionSupported( extension ), "Extension is not supported\n" );

    create_info.enabledExtensionCount = static_cast<u32>( extensions.size() );
    create_info.ppEnabledExtensionNames = extensions.data();

    VkDebugUtilsMessengerCreateInfoEXT debug_create_info;

    if ( m_enableValidationLayers ) {
      create_info.enabledLayerCount = static_cast<u32>( m_validationLayers.size() );
      create_info.ppEnabledLayerNames = m_validationLayers.data();

      populateDebugMessengerCreateInfo( debug_create_info );
      create_info.pNext = ( VkDebugUtilsMessengerCallbackDataEXT * )&debug_create_info;

    } else {
      create_info.enabledLayerCount = 0;
      create_info.pNext = nullptr;
    }


    VK_CHECK_RESULT( vkCreateInstance( &create_info, nullptr, &m_vulkanInstance ) );
  }    // namespace nile

  void VulkanRenderingDevice::submitFrame() noexcept {}

  void VulkanRenderingDevice::endFrame() noexcept {
    // swap buffers
  }

  bool VulkanRenderingDevice::checkValidationLayerSupport() const noexcept {
    u32 layer_count;
    vkEnumerateInstanceLayerProperties( &layer_count, nullptr );

    std::vector<VkLayerProperties> available_layers( layer_count );
    vkEnumerateInstanceLayerProperties( &layer_count, available_layers.data() );

    for ( const char *layerName : m_validationLayers ) {

      bool layer_found = false;

      for ( const auto &layerProperties : available_layers ) {
        if ( strcmp( layerName, layerProperties.layerName ) == 0 ) {
          layer_found = true;
          break;
        }
      }

      if ( !layer_found ) {
        return false;
      }
    }

    return true;
  }

  std::vector<const char *> VulkanRenderingDevice::getRequiredExtensions() const noexcept {

    u32 extensions_count = 0;
    ASSERT_M( SDL_Vulkan_GetInstanceExtensions( m_window, &extensions_count, nullptr ),
              "Could not find any SDL2 Instance extensions\n" );
    std::vector<const char *> extensions( extensions_count );
    ASSERT_M( SDL_Vulkan_GetInstanceExtensions( m_window, &extensions_count, extensions.data() ),
              "Could not find any SDL2 Instance extensions\n" );

    if ( m_enableValidationLayers ) {
      extensions.push_back( VK_EXT_DEBUG_UTILS_EXTENSION_NAME );
    }
    return extensions;
  }

  VKAPI_ATTR VkBool32 VKAPI_CALL VulkanRenderingDevice::debugCallback(
      VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
      VkDebugUtilsMessageTypeFlagsEXT messageType,
      const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData, void *pUserData ) noexcept {


    log::error( "Validation layer: %s\n", pCallbackData->pMessage );

    return VK_FALSE;
  }

  void VulkanRenderingDevice::setupDebugMessenger() noexcept {

    if ( !m_enableValidationLayers )
      return;

    VkDebugUtilsMessengerCreateInfoEXT create_info = {};
    populateDebugMessengerCreateInfo( create_info );

    VK_CHECK_RESULT( CreateDebugUtilsMessengerEXT( m_vulkanInstance, &create_info, nullptr,
                                                   &m_debugMessenger ) );
  }

  void VulkanRenderingDevice::populateDebugMessengerCreateInfo(
      VkDebugUtilsMessengerCreateInfoEXT &createInfo ) noexcept {

    createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
                                 VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
                                 VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
                             VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                             VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    createInfo.pfnUserCallback = debugCallback;
  }

  void VulkanRenderingDevice::pickPhysicalDevice() noexcept {

    u32 device_count = 0;
    vkEnumeratePhysicalDevices( m_vulkanInstance, &device_count, nullptr );
    ASSERT_M( ( device_count >= 1 ), "Failed to find GPUs with Vulkan Support!\n" );

    std::vector<VkPhysicalDevice> devices( device_count );
    vkEnumeratePhysicalDevices( m_vulkanInstance, &device_count, devices.data() );

    for ( const auto &device : devices ) {
      if ( isDeviceSuitable( device ) ) {
        m_physicalDevice = device;
        break;
      }
    }

    ASSERT_M( ( m_physicalDevice != VK_NULL_HANDLE ), "Failed to find suitable GPU!\n" );
  }

  bool VulkanRenderingDevice::isDeviceSuitable( VkPhysicalDevice device ) noexcept {

    // VkPhysicalDeviceProperties device_properties;
    // VkPhysicalDeviceFeatures device_features;
    // vkGetPhysicalDeviceProperties(device, &device_properties);
    // vkGetPhysicalDeviceFeatures(device, &device_features);

    QueueFamilyIndices indices = findQueueFamilies( device );

    return indices.isComplete();
  }

  VulkanRenderingDevice::QueueFamilyIndices
  VulkanRenderingDevice::findQueueFamilies( VkPhysicalDevice device ) noexcept {

    QueueFamilyIndices indices;

    u32 queue_family_count = 0;
    vkGetPhysicalDeviceQueueFamilyProperties( device, &queue_family_count, nullptr );

    std::vector<VkQueueFamilyProperties> queue_families( queue_family_count );
    vkGetPhysicalDeviceQueueFamilyProperties( device, &queue_family_count, queue_families.data() );

    u32 queue_count = 0;

    for ( const auto &queue_family : queue_families ) {

      if ( queue_family.queueFlags & VK_QUEUE_GRAPHICS_BIT ) {
        indices.graphicsFamily = queue_count;
      }

      VkBool32 present_support = false;
      vkGetPhysicalDeviceSurfaceSupportKHR( device, queue_count, m_surface, &present_support );

      if ( present_support ) {
        indices.presentFamily = queue_count;
      }


      if ( indices.isComplete() )
        break;

      queue_count++;
    }

    return indices;
  }

  void VulkanRenderingDevice::createLogicalDevice() noexcept {

    QueueFamilyIndices indices = findQueueFamilies( m_physicalDevice );

    std::vector<VkDeviceQueueCreateInfo> queue_create_infos;
    std::set<u32> unique_queue_families = {indices.graphicsFamily.value(),
                                           indices.presentFamily.value()};

    f32 queue_priority = 1.0f;

    for ( u32 queue_family : unique_queue_families ) {
      VkDeviceQueueCreateInfo queue_create_info = {};
      queue_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
      queue_create_info.queueFamilyIndex = queue_family;
      queue_create_info.queueCount = 1;
      queue_create_info.pQueuePriorities = &queue_priority;
      queue_create_infos.push_back( queue_create_info );
    }

    VkPhysicalDeviceFeatures device_features = {};

    VkDeviceCreateInfo create_info = {};
    create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    create_info.pQueueCreateInfos = queue_create_infos.data();
    create_info.queueCreateInfoCount = static_cast<u32>( queue_create_infos.size() );
    create_info.pEnabledFeatures = &device_features;
    create_info.enabledExtensionCount = 0;

    if ( m_enableValidationLayers ) {
      create_info.enabledLayerCount = static_cast<u32>( m_validationLayers.size() );
      create_info.ppEnabledLayerNames = m_validationLayers.data();
    } else {
      create_info.enabledLayerCount = 0;
    }

    VK_CHECK_RESULT( vkCreateDevice( m_physicalDevice, &create_info, nullptr, &m_logicalDevice ) );

    vkGetDeviceQueue( m_logicalDevice, indices.graphicsFamily.value(), 0, &m_graphicsQueue );
    vkGetDeviceQueue( m_logicalDevice, indices.presentFamily.value(), 0, &m_presentQueue );
  }

  void VulkanRenderingDevice::createSurface() noexcept {
    ASSERT_M( SDL_Vulkan_CreateSurface( m_window, m_vulkanInstance, &m_surface ),
              "Failed to create window surface\n" );
  }

}    // namespace nile

