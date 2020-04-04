#include "Nile/drivers/vulkan/vulkan_rendering_device.hh"
#include "Nile/asset/asset_manager.hh"
#include "Nile/core/assert.hh"
#include "Nile/core/file_system.hh"
#include "Nile/core/settings.hh"
#include "Nile/core/types.hh"
#include "Nile/drivers/vulkan/vulkan_device.hh"
#include "Nile/drivers/vulkan/vulkan_utils.hh"
#include "Nile/log/log.hh"
#include "stb_image.h"


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

  VulkanRenderingDevice::VulkanRenderingDevice(
      const std::shared_ptr<Settings> &settings,
      const std::shared_ptr<AssetManager> &assetManager ) noexcept
      : RenderingDevice( settings )
      , m_assetManager( assetManager ) {}

  void VulkanRenderingDevice::initialize() noexcept {

    this->initRenderingInternal();
    this->initWindow();
    this->createVulkanInstance();
    this->createSurface();
    this->setupDebugMessenger();
    this->pickPhysicalDevice();
    this->createLogicalDevice();
    this->createSwapChain();
    this->createImageViews();
    this->createTextureSampler();
    this->createRenderPass();
    this->createDescriptorSetLayout();
    this->createGraphicsPipeline();
    this->createDepthResources();
    this->createFrameBuffers();
    this->createCommandPool();
    this->createTextureImage();
    this->createTextureImageView();
    this->createVertexBuffer();
    this->createIndexBuffer();
    this->createUniformBuffers();
    this->createDescriptorPool();
    this->createDescriptorSets();
    this->createCommandBuffers();
    this->createSyncObjects();
  }

  void VulkanRenderingDevice::destory() noexcept {

    this->cleanupSwapChain();

    vkDestroySampler( m_device->getDevice(), m_textureSampler, nullptr );
    vkDestroyImageView( m_device->getDevice(), m_textureImageView, nullptr );
    vkDestroyImage( m_device->getDevice(), m_textureImage, nullptr );
    vkFreeMemory( m_device->getDevice(), m_textureImageMemory, nullptr );

    vkDestroyDescriptorSetLayout( m_device->getDevice(), m_descriptorSetLayout, nullptr );

    m_indexBuffer.destory();
    m_vertexBuffer.destory();

    for ( size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++ ) {
      vkDestroySemaphore( m_device->getDevice(), m_semaphores.renderingHasFinished[ i ], nullptr );
      vkDestroySemaphore( m_device->getDevice(), m_semaphores.imageIsAvailable[ i ], nullptr );
      vkDestroyFence( m_device->getDevice(), m_inFlightFences[ i ], nullptr );
    }

    vkDestroyCommandPool( m_device->getDevice(), m_commandPool, nullptr );

    delete m_device;

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

  void VulkanRenderingDevice::submitFrame() noexcept {

    // @info: submitFrame will perform every frame the following operations:
    // - aquire an image from the swap chain
    // - execute the command buffer with that image as attachment in the framebuffer
    // - return the image to the swap chain for presentation

    //@deprecated
    //@deprecated
    //@deprecated
    //@deprecated
    
    vkWaitForFences( m_device->getDevice(), 1, &m_inFlightFences[ m_currentFrame ], VK_TRUE,
                     UINT64_MAX );


    auto result = vkAcquireNextImageKHR( m_device->getDevice(), m_swapChain, UINT64_MAX,
                                         m_semaphores.imageIsAvailable[ m_currentFrame ],
                                         VK_NULL_HANDLE, &m_imageIndex );

    if ( result == VK_ERROR_OUT_OF_DATE_KHR ) {
      this->recreateSwapChain();
      return;
    }

    this->updateUniformBuffer( m_imageIndex );

    ASSERT_M( result == VK_SUCCESS || result == VK_SUBOPTIMAL_KHR,
              "Failed to acquire swap chain image" );

    // Check if previous frame is using this image ( i.e. there is its fence to wait on)
    if ( m_imagesInFlight[ m_imageIndex ] != VK_NULL_HANDLE ) {
      vkWaitForFences( m_device->getDevice(), 1, &m_imagesInFlight[ m_imageIndex ], VK_TRUE,
                       UINT64_MAX );
    }

    // mark the image as now being in use by this frame
    m_imagesInFlight[ m_imageIndex ] = m_inFlightFences[ m_currentFrame ];

    // submitting the command buffer
    m_submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    VkSemaphore wait_semaphores[] = {m_semaphores.imageIsAvailable[ m_currentFrame ]};
    VkPipelineStageFlags wait_stages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
    m_submit_info.waitSemaphoreCount = 1;
    m_submit_info.pWaitSemaphores = wait_semaphores;
    m_submit_info.pWaitDstStageMask = wait_stages;
    m_submit_info.commandBufferCount = 1;
    m_submit_info.pCommandBuffers = &m_commandBuffers[ m_imageIndex ];

    VkSemaphore signal_semaphores[] = {m_semaphores.renderingHasFinished[ m_currentFrame ]};
    m_submit_info.signalSemaphoreCount = 1;
    m_submit_info.pSignalSemaphores = signal_semaphores;

    vkResetFences( m_device->getDevice(), 1, &m_inFlightFences[ m_currentFrame ] );

    
    // @note: m_inFlightFences[m_currentFrame] -> signal that the frame has finished
    VK_CHECK_RESULT(
        vkQueueSubmit( m_graphicsQueue, 1, &m_submit_info, m_inFlightFences[ m_currentFrame ] ) );

    VkPresentInfoKHR present_info = {};
    present_info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    present_info.waitSemaphoreCount = 1;
    present_info.pWaitSemaphores = signal_semaphores;

    VkSwapchainKHR swap_chains[] = {m_swapChain};
    present_info.swapchainCount = 1;
    present_info.pSwapchains = swap_chains;
    present_info.pImageIndices = &m_imageIndex;
    present_info.pResults = nullptr;

    result = vkQueuePresentKHR( m_presentQueue, &present_info );

    if ( result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR ||
         m_frambufferResized ) {
      m_frambufferResized = false;
      recreateSwapChain();
    } else if ( result != VK_SUCCESS ) {
      ASSERT_M( false, "Failed to present swap chain image \n" );
    }

    m_currentFrame = ( m_currentFrame + 1 ) % MAX_FRAMES_IN_FLIGHT;
  }

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

  bool VulkanRenderingDevice::isDeviceSuitable( VkPhysicalDevice device ) const noexcept {

    bool extension_is_supported = checkDeviceExtensionSuport( device );

    VkPhysicalDeviceFeatures supported_features;
    vkGetPhysicalDeviceFeatures( device, &supported_features );

    bool swap_chain_adequate = false;
    if ( extension_is_supported ) {
      auto swap_chain_support = querySwapChainSupport( device );
      swap_chain_adequate =
          !swap_chain_support.formats.empty() && !swap_chain_support.presentModes.empty();
    }

    return extension_is_supported && swap_chain_adequate && supported_features.samplerAnisotropy;
  }

  bool VulkanRenderingDevice::checkDeviceExtensionSuport( VkPhysicalDevice device ) const noexcept {

    u32 extensions_count;
    vkEnumerateDeviceExtensionProperties( device, nullptr, &extensions_count, nullptr );

    std::vector<VkExtensionProperties> available_extensions( extensions_count );
    vkEnumerateDeviceExtensionProperties( device, nullptr, &extensions_count,
                                          available_extensions.data() );

    std::set<std::string> required_extensions( m_deviceExtensions.begin(),
                                               m_deviceExtensions.end() );

    for ( const auto &extension : available_extensions ) {
      required_extensions.erase( extension.extensionName );
    }

    return required_extensions.empty();
  }

  void VulkanRenderingDevice::createLogicalDevice() noexcept {

    m_device = new VulkanDevice( m_physicalDevice );

    VkPhysicalDeviceFeatures device_features = {};
    device_features.samplerAnisotropy = VK_TRUE;

    VK_CHECK_RESULT( m_device->createLogicalDevice( device_features, m_deviceExtensions ) );

    // @CHECK WHAT TO DO WITH THIS
    // @CHECK WHAT TO DO WITH THIS
    // @CHECK WHAT TO DO WITH THIS
    vkGetDeviceQueue( m_device->getDevice(), m_device->getQueueFamilyIndices().graphics, 0,
                      &m_graphicsQueue );
    VkBool32 present_support = VK_FALSE;
    vkGetPhysicalDeviceSurfaceSupportKHR(
        m_physicalDevice, m_device->getQueueFamilyIndices().graphics, m_surface, &present_support );

    if ( present_support ) {
      vkGetDeviceQueue( m_device->getDevice(), m_device->getQueueFamilyIndices().present, 0,
                        &m_presentQueue );
    } else {
      log::error( "Present QUEUE is not supported\n" );
    }
  }

  void VulkanRenderingDevice::createSurface() noexcept {
    ASSERT_M( SDL_Vulkan_CreateSurface( m_window, m_vulkanInstance, &m_surface ),
              "Failed to create window surface\n" );
  }

  [[nodiscard]] VulkanRenderingDevice::SwapChainSupportDetails
  VulkanRenderingDevice::querySwapChainSupport( VkPhysicalDevice device ) const noexcept {

    SwapChainSupportDetails details;

    vkGetPhysicalDeviceSurfaceCapabilitiesKHR( device, m_surface, &details.capabilities );

    u32 format_count;
    vkGetPhysicalDeviceSurfaceFormatsKHR( device, m_surface, &format_count, nullptr );

    if ( format_count != 0 ) {
      details.formats.resize( format_count );
      vkGetPhysicalDeviceSurfaceFormatsKHR( device, m_surface, &format_count,
                                            details.formats.data() );
    }

    u32 present_mode_count;
    vkGetPhysicalDeviceSurfacePresentModesKHR( device, m_surface, &present_mode_count, nullptr );

    if ( present_mode_count != 0 ) {
      details.presentModes.resize( present_mode_count );
      vkGetPhysicalDeviceSurfacePresentModesKHR( device, m_surface, &present_mode_count,
                                                 details.presentModes.data() );
    }

    return details;
  }
  [[nodiscard]] VkSurfaceFormatKHR VulkanRenderingDevice::chooseSwapSurfaceFormat(
      const std::vector<VkSurfaceFormatKHR> &availableFormats ) noexcept {

    for ( const auto &available_format : availableFormats ) {
      if ( available_format.format == VK_FORMAT_R8G8B8A8_SRGB &&
           available_format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR ) {
        return available_format;
      }
    }

    // If the above fails, return the first available format ( in most cases this is ok
    // but we could write a rankning system to choose the format)
    return availableFormats[ 0 ];
  }

  [[nodiscard]] VkPresentModeKHR VulkanRenderingDevice::chooseSwapPresentMode(
      const std::vector<VkPresentModeKHR> &availablePresentModes ) noexcept {

    // four possible presentation modes available in vulkan:
    // 1.) VK_PRESENT_MODE_IMMEDIATE_KHR : images are submited and transfered to
    //     the screen right away, which may resulting in tearing.
    // 2.) VK_PRESENT_MODE_FIFO_KHR : The swap chain is a queue where the display is
    //     take an image form the front of the queue, and the rendering image is pushed
    //     to the back of the queue. If the queue is full then the program has to wait.
    //     this is most similar to vertical sync.
    // 3.) VK_PRESENT_MODE_FIFO_RELAXED_KHR: The mode is only differs from the previous one
    //     in that if the application is late and the queue was empty at the last
    //     vertical blank, instead of waiting the image is transfered right away when in
    //     it finialy arives. this may cause visible tearing.
    // 4.) VK_PRESENT_MODE_MAILBOX_KHR: this is another variation of the second mode
    //     Instead of blocking the application when the queue is full, the images that are
    //     already qued are simply replaced with the newer ones. This mode can be used to
    //     implement triple buffering.

    for ( const auto &available_present_mode : availablePresentModes ) {
      if ( available_present_mode == VK_PRESENT_MODE_MAILBOX_KHR ) {
        return available_present_mode;
      }
    }

    // the VK_PRESENT_MODE_FIFO_KHR mode is guaranteed to be available, so in case of
    // failure we return this
    return VK_PRESENT_MODE_FIFO_KHR;
  }

  [[nodiscard]] VkExtent2D
  VulkanRenderingDevice::chooseSwapExtent( const VkSurfaceCapabilitiesKHR &capabilities,
                                           const std::shared_ptr<Settings> &settings ) noexcept {

    if ( capabilities.currentExtent.width != std::numeric_limits<u32>::max() ) {
      return capabilities.currentExtent;
    } else {

      i32 width, height;
      SDL_GetWindowSize( m_window, &width, &height );

      // VkExtent2D actual_extent = {settings->getWidth(), settings->getHeight()};
      VkExtent2D actual_extent = {static_cast<u32>( width ), static_cast<u32>( height )};

      actual_extent.width = std::clamp( actual_extent.width, capabilities.minImageExtent.width,
                                        capabilities.maxImageExtent.width );
      actual_extent.height = std::clamp( actual_extent.height, capabilities.minImageExtent.height,
                                         capabilities.maxImageExtent.height );

      return actual_extent;
    }
  }

  void VulkanRenderingDevice::createSwapChain() noexcept {

    auto swap_chain_support = querySwapChainSupport( m_physicalDevice );

    auto surface_format = chooseSwapSurfaceFormat( swap_chain_support.formats );
    auto present_mode = chooseSwapPresentMode( swap_chain_support.presentModes );
    auto extent = chooseSwapExtent( swap_chain_support.capabilities, m_settings );

    u32 image_count = swap_chain_support.capabilities.minImageCount + 1;

    if ( swap_chain_support.capabilities.maxImageCount > 0 &&
         image_count > swap_chain_support.capabilities.maxImageCount ) {
      image_count = swap_chain_support.capabilities.maxImageCount;
    }

    VkSwapchainCreateInfoKHR create_info = {};
    create_info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    create_info.surface = m_surface;
    create_info.minImageCount = image_count;
    create_info.imageFormat = surface_format.format;
    create_info.imageColorSpace = surface_format.colorSpace;
    create_info.imageExtent = extent;
    create_info.imageArrayLayers = 1;
    // we are going to render directly to images, that's why we are using
    // color attachment, if we want to do some post-procssing we would use
    // VK_IMAGE_USAGE_TRANSFER_DST_BIT
    create_info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    auto indices = m_device->getQueueFamilyIndices();
    u32 queue_family_indices[] = {indices.graphics, indices.present};

    if ( indices.graphics != indices.present ) {
      create_info.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
      create_info.queueFamilyIndexCount = 2;
      create_info.pQueueFamilyIndices = queue_family_indices;
    } else {
      create_info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
      create_info.queueFamilyIndexCount = 0;
      create_info.pQueueFamilyIndices = nullptr;
    }

    create_info.preTransform = swap_chain_support.capabilities.currentTransform;
    create_info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    create_info.presentMode = present_mode;
    create_info.clipped = VK_TRUE;

    // We should use this when we will recreate the swapchain, in case of window resizing
    create_info.oldSwapchain = nullptr;

    VK_CHECK_RESULT(
        vkCreateSwapchainKHR( m_device->getDevice(), &create_info, nullptr, &m_swapChain ) );

    vkGetSwapchainImagesKHR( m_device->getDevice(), m_swapChain, &image_count, nullptr );
    m_swapChainImages.resize( image_count );
    vkGetSwapchainImagesKHR( m_device->getDevice(), m_swapChain, &image_count,
                             m_swapChainImages.data() );

    m_swapChainImageFormat = surface_format.format;
    m_swapChainExtent = extent;
  }

  void VulkanRenderingDevice::createImageViews() noexcept {

    m_sawapChainImageViews.resize( m_swapChainImages.size() );

    for ( size_t i = 0; i < m_swapChainImages.size(); i++ ) {

      m_sawapChainImageViews[ i ] = createImageView( m_swapChainImages[ i ], m_swapChainImageFormat,
                                                     VK_IMAGE_ASPECT_COLOR_BIT );
    }
  }

  void VulkanRenderingDevice::createRenderPass() noexcept {

    VkAttachmentDescription color_attachment = {};
    color_attachment.format = m_swapChainImageFormat;
    color_attachment.samples = VK_SAMPLE_COUNT_1_BIT;

    // Color depth buffer
    color_attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    color_attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;

    // We don't do anything with stencil data yeat ( maybe we will be using this
    // in the editor )
    color_attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    color_attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;

    color_attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    color_attachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    VkAttachmentReference color_attachment_ref = {};
    color_attachment_ref.attachment = 0;
    color_attachment_ref.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkAttachmentDescription depth_attachment = {};
    depth_attachment.format = findDepthFormat();
    depth_attachment.samples = VK_SAMPLE_COUNT_1_BIT;
    depth_attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    depth_attachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    depth_attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    depth_attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    depth_attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    depth_attachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    VkAttachmentReference depth_attachment_ref = {};
    depth_attachment_ref.attachment = 1;
    depth_attachment_ref.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    VkSubpassDescription subpass = {};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &color_attachment_ref;
    subpass.pDepthStencilAttachment = &depth_attachment_ref;

    VkSubpassDependency dependency = {};
    dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    dependency.dstSubpass = 0;
    dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.srcAccessMask = 0;
    dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.dstAccessMask =
        VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

    std::array<VkAttachmentDescription, 2> attachments = {color_attachment, depth_attachment};
    VkRenderPassCreateInfo render_pass_info = {};
    render_pass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    render_pass_info.attachmentCount = static_cast<u32>( attachments.size() );
    render_pass_info.pAttachments = attachments.data();
    render_pass_info.subpassCount = 1;
    render_pass_info.pSubpasses = &subpass;
    render_pass_info.dependencyCount = 1;
    render_pass_info.pDependencies = &dependency;

    VK_CHECK_RESULT(
        vkCreateRenderPass( m_device->getDevice(), &render_pass_info, nullptr, &m_renderPass ) );
  }

  void VulkanRenderingDevice::createGraphicsPipeline() noexcept {

    auto vertex_shader_result = FileSystem::readFile( "../assets/vk_shaders/shader.vert.spv" );
    auto fragment_shader_result = FileSystem::readFile( "../assets/vk_shaders/shader.frag.spv" );

    auto vertex_shader_code = std::get_if<std::vector<char>>( &vertex_shader_result );
    auto fragment_shader_code = std::get_if<std::vector<char>>( &fragment_shader_result );

    ASSERT_M( vertex_shader_code,
              errorToString( std::get<ErrorCode>( vertex_shader_result ) ).c_str() );
    ASSERT_M( fragment_shader_code,
              errorToString( std::get<ErrorCode>( fragment_shader_result ) ).c_str() );

    auto vertex_shader_module = createShaderModule( *vertex_shader_code );
    auto fragment_shader_module = createShaderModule( *fragment_shader_code );

    VkPipelineShaderStageCreateInfo vert_shader_stage_create_info = {};
    vert_shader_stage_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    vert_shader_stage_create_info.stage = VK_SHADER_STAGE_VERTEX_BIT;
    vert_shader_stage_create_info.module = vertex_shader_module;
    vert_shader_stage_create_info.pName = "main";
    // pSpecializationInfo allows you to specify values for shader constants.
    // this make the code more efficient rathen thatn settings constants at runtime
    vert_shader_stage_create_info.pSpecializationInfo = nullptr;

    VkPipelineShaderStageCreateInfo frag_shader_stage_create_info = {};
    frag_shader_stage_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    frag_shader_stage_create_info.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    frag_shader_stage_create_info.module = fragment_shader_module;
    frag_shader_stage_create_info.pName = "main";
    frag_shader_stage_create_info.pSpecializationInfo = nullptr;

    VkPipelineShaderStageCreateInfo shader_stages[] = {vert_shader_stage_create_info,
                                                       frag_shader_stage_create_info};

    auto binding_description = VulkanVertex::getBindingDescription();
    auto attribute_descriptions = VulkanVertex::getAttributeDescriptions();

    VkPipelineVertexInputStateCreateInfo vertex_input_info = {};
    vertex_input_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertex_input_info.vertexBindingDescriptionCount = 1;
    vertex_input_info.pVertexBindingDescriptions = &binding_description;
    vertex_input_info.vertexAttributeDescriptionCount =
        static_cast<u32>( attribute_descriptions.size() );
    vertex_input_info.pVertexAttributeDescriptions = attribute_descriptions.data();

    VkPipelineInputAssemblyStateCreateInfo input_assembly = {};
    input_assembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    // @tip: what kind of gemotetry will be drawn?
    input_assembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    input_assembly.primitiveRestartEnable = VK_FALSE;

    VkViewport view_port = {};
    view_port.x = 0.0f;
    view_port.y = 0.0f;
    view_port.width = static_cast<f32>( m_swapChainExtent.width );
    view_port.height = static_cast<f32>( m_swapChainExtent.height );
    view_port.minDepth = 0.0f;
    view_port.maxDepth = 1.0f;

    VkRect2D scissor = {};
    scissor.offset = {0, 0};
    scissor.extent = m_swapChainExtent;

    VkPipelineViewportStateCreateInfo view_port_state = {};
    view_port_state.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    view_port_state.viewportCount = 1;
    view_port_state.pViewports = &view_port;
    view_port_state.scissorCount = 1;
    view_port_state.pScissors = &scissor;

    VkPipelineRasterizationStateCreateInfo rasterizer = {};
    rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterizer.depthClampEnable = VK_FALSE;
    rasterizer.rasterizerDiscardEnable = VK_FALSE;
    rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
    rasterizer.lineWidth = 1.0f;
    rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
    rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;

    rasterizer.depthBiasEnable = VK_FALSE;
    rasterizer.depthBiasConstantFactor = 0.0f;
    rasterizer.depthBiasClamp = 0.0f;
    rasterizer.depthBiasSlopeFactor = 0.0f;

    VkPipelineMultisampleStateCreateInfo multisampling = {};
    multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisampling.sampleShadingEnable = VK_FALSE;
    multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
    multisampling.minSampleShading = 1.0f;
    multisampling.pSampleMask = nullptr;
    multisampling.alphaToCoverageEnable = VK_FALSE;
    multisampling.alphaToOneEnable = VK_FALSE;

    VkPipelineDepthStencilStateCreateInfo depth_stencil = {};
    depth_stencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    depth_stencil.depthTestEnable = VK_TRUE;
    depth_stencil.depthWriteEnable = VK_TRUE;
    depth_stencil.depthCompareOp = VK_COMPARE_OP_LESS;
    depth_stencil.depthBoundsTestEnable = VK_FALSE;
    depth_stencil.stencilTestEnable = VK_FALSE;

    VkPipelineColorBlendAttachmentState color_blend_attachment = {};
    color_blend_attachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
                                            VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    // @disabled: alpha blending is didabled
    color_blend_attachment.blendEnable = VK_FALSE;
    color_blend_attachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
    color_blend_attachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
    color_blend_attachment.colorBlendOp = VK_BLEND_OP_ADD;
    color_blend_attachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
    color_blend_attachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
    color_blend_attachment.alphaBlendOp = VK_BLEND_OP_ADD;


    // the folowing code is to enable alpha blending
    // color_blend_attachment.blendEnable = VK_TRUE;
    // color_blend_attachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
    // color_blend_attachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
    // color_blend_attachment.colorBlendOp = VK_BLEND_OP_ADD;
    // color_blend_attachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
    // color_blend_attachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
    // color_blend_attachment.alphaBlendOp = VK_BLEND_OP_ADD;

    VkPipelineColorBlendStateCreateInfo color_blending = {};
    color_blending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    color_blending.logicOpEnable = VK_FALSE;
    color_blending.logicOp = VK_LOGIC_OP_COPY;
    color_blending.attachmentCount = 1;
    color_blending.pAttachments = &color_blend_attachment;
    color_blending.blendConstants[ 0 ] = 0.0f;
    color_blending.blendConstants[ 1 ] = 0.0f;
    color_blending.blendConstants[ 2 ] = 0.0f;
    color_blending.blendConstants[ 3 ] = 0.0f;

    // @brief: we specify here descriptor sets ( uniform buffers ) or push constants
    // to use inside our shaders
    VkPipelineLayoutCreateInfo pipeline_layotu_info = {};
    pipeline_layotu_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipeline_layotu_info.setLayoutCount = 1;
    pipeline_layotu_info.pSetLayouts = &m_descriptorSetLayout;
    pipeline_layotu_info.pushConstantRangeCount = 0;
    pipeline_layotu_info.pPushConstantRanges = nullptr;

    VK_CHECK_RESULT( vkCreatePipelineLayout( m_device->getDevice(), &pipeline_layotu_info, nullptr,
                                             &m_pipelineLayout ) );

    VkGraphicsPipelineCreateInfo pipeline_info = {};
    pipeline_info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipeline_info.stageCount = 2;
    pipeline_info.pStages = shader_stages;
    pipeline_info.pVertexInputState = &vertex_input_info;
    pipeline_info.pInputAssemblyState = &input_assembly;
    pipeline_info.pViewportState = &view_port_state;
    pipeline_info.pRasterizationState = &rasterizer;
    pipeline_info.pMultisampleState = &multisampling;
    pipeline_info.pDepthStencilState = &depth_stencil;
    pipeline_info.pColorBlendState = &color_blending;
    pipeline_info.pDynamicState = nullptr;

    pipeline_info.layout = m_pipelineLayout;
    pipeline_info.renderPass = m_renderPass;
    pipeline_info.subpass = 0;
    pipeline_info.basePipelineHandle = VK_NULL_HANDLE;
    pipeline_info.basePipelineIndex = -1;

    VK_CHECK_RESULT( vkCreateGraphicsPipelines( m_device->getDevice(), VK_NULL_HANDLE, 1,
                                                &pipeline_info, nullptr, &m_graphicsPipeline ) );


    vkDestroyShaderModule( m_device->getDevice(), vertex_shader_module, nullptr );
    vkDestroyShaderModule( m_device->getDevice(), fragment_shader_module, nullptr );
  }

  [[nodiscard]] VkShaderModule
  VulkanRenderingDevice::createShaderModule( const std::vector<char> &code ) noexcept {

    VkShaderModuleCreateInfo create_info = {};
    create_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    create_info.codeSize = code.size();
    create_info.pCode = reinterpret_cast<const u32 *>( code.data() );

    VkShaderModule shader_module;

    VK_CHECK_RESULT(
        vkCreateShaderModule( m_device->getDevice(), &create_info, nullptr, &shader_module ) );

    return shader_module;
  }

  void VulkanRenderingDevice::createFrameBuffers() noexcept {

    m_swapChainFrameBuffers.resize( m_sawapChainImageViews.size() );

    for ( size_t i = 0; i < m_sawapChainImageViews.size(); i++ ) {

      std::array<VkImageView, 2> attachments = {m_sawapChainImageViews[ i ], m_depthImageView};

      VkFramebufferCreateInfo framebuffer_info = {};
      framebuffer_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
      framebuffer_info.renderPass = m_renderPass;
      framebuffer_info.attachmentCount = static_cast<u32>( attachments.size() );
      framebuffer_info.pAttachments = attachments.data();
      framebuffer_info.width = m_swapChainExtent.width;
      framebuffer_info.height = m_swapChainExtent.height;
      framebuffer_info.layers = 1;

      VK_CHECK_RESULT( vkCreateFramebuffer( m_device->getDevice(), &framebuffer_info, nullptr,
                                            &m_swapChainFrameBuffers[ i ] ) );
    }
  }

  //@derpecated
  //@derpecated
  //@derpecated
  void VulkanRenderingDevice::createCommandPool() noexcept {


    VkCommandPoolCreateInfo pool_info = {};
    pool_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;

    // Command buffers are executed by submitting them on one of the device queues, like
    // the graphics or presentaion queues.
    pool_info.queueFamilyIndex = m_device->getQueueFamilyIndices().graphics;
    pool_info.flags = 0;

    VK_CHECK_RESULT(
        vkCreateCommandPool( m_device->getDevice(), &pool_info, nullptr, &m_commandPool ) );
  }


  // @this will be moved inside swapchain class
  void VulkanRenderingDevice::createCommandBuffers() noexcept {

    m_commandBuffers.resize( m_swapChainFrameBuffers.size() );

    VkCommandBufferAllocateInfo alloc_info = {};
    alloc_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    alloc_info.commandPool = m_commandPool;
    // VK_COMMAND_BUFFER_LEVEL_PRIMARY : can be submitted to a queue for execution, but
    // cannot be called from other command buffers
    // VK_COMMAND_BUFFER_LEVEL_SECONDARY: cannot be submitted to a queue directly, but can
    // be called from primary command buffers
    alloc_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    alloc_info.commandBufferCount = static_cast<u32>( m_commandBuffers.size() );

    VK_CHECK_RESULT(
        vkAllocateCommandBuffers( m_device->getDevice(), &alloc_info, m_commandBuffers.data() ) );

    // @ start the recording of the command buffers
    for ( size_t i = 0; i < m_commandBuffers.size(); i++ ) {
      VkCommandBufferBeginInfo begin_info = {};
      begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
      begin_info.flags = 0;
      begin_info.pInheritanceInfo = nullptr;

      VK_CHECK_RESULT( vkBeginCommandBuffer( m_commandBuffers[ i ], &begin_info ) );

      VkRenderPassBeginInfo render_pass_info = {};
      render_pass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
      render_pass_info.renderPass = m_renderPass;
      render_pass_info.framebuffer = m_swapChainFrameBuffers[ i ];
      render_pass_info.renderArea.offset = {0, 0};
      render_pass_info.renderArea.extent = m_swapChainExtent;

      std::array<VkClearValue, 2> clear_values = {};
      clear_values[ 0 ].color = {{0.0f, 0.0f, 0.0f, 1.0f}};
      clear_values[ 1 ].depthStencil = {1.0f, 0};

      render_pass_info.clearValueCount = static_cast<u32>( clear_values.size() );
      render_pass_info.pClearValues = clear_values.data();

      // Begin the recording
      vkCmdBeginRenderPass( m_commandBuffers[ i ], &render_pass_info, VK_SUBPASS_CONTENTS_INLINE );

      vkCmdBindPipeline( m_commandBuffers[ i ], VK_PIPELINE_BIND_POINT_GRAPHICS,
                         m_graphicsPipeline );

      VkBuffer vertexBuffers[] = {m_vertexBuffer.buffer};
      VkDeviceSize offsets[] = {0};
      vkCmdBindVertexBuffers( m_commandBuffers[ i ], 0, 1, vertexBuffers, offsets );
      vkCmdBindIndexBuffer( m_commandBuffers[ i ], m_indexBuffer.buffer, 0, VK_INDEX_TYPE_UINT16 );

      vkCmdBindDescriptorSets( m_commandBuffers[ i ], VK_PIPELINE_BIND_POINT_GRAPHICS,
                               m_pipelineLayout, 0, 1, &m_descriptorSets[ i ], 0, nullptr );

      // Draw a triangle
      // vkCmdDraw( m_commandBuffers[ i ], static_cast<u32>( m_vertices.size() ), 1, 0, 0 );
      vkCmdDrawIndexed( m_commandBuffers[ i ], static_cast<u32>( m_indicesSize ), 1, 0, 0, 0 );

      vkCmdEndRenderPass( m_commandBuffers[ i ] );

      // Finish the recording
      VK_CHECK_RESULT( vkEndCommandBuffer( m_commandBuffers[ i ] ) );
    }
  }

  void VulkanRenderingDevice::createSyncObjects() noexcept {

    m_semaphores.imageIsAvailable.resize( MAX_FRAMES_IN_FLIGHT );
    m_semaphores.renderingHasFinished.resize( MAX_FRAMES_IN_FLIGHT );
    m_inFlightFences.resize( MAX_FRAMES_IN_FLIGHT );
    m_imagesInFlight.resize( m_swapChainImages.size(), VK_NULL_HANDLE );
    VkSemaphoreCreateInfo semaphore_info = {};
    semaphore_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    VkFenceCreateInfo fence_info = {};
    fence_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fence_info.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    for ( size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++ ) {
      VK_CHECK_RESULT( vkCreateSemaphore( m_device->getDevice(), &semaphore_info, nullptr,
                                          &m_semaphores.imageIsAvailable[ i ] ) );

      VK_CHECK_RESULT( vkCreateSemaphore( m_device->getDevice(), &semaphore_info, nullptr,
                                          &m_semaphores.renderingHasFinished[ i ] ) );

      //@deprecated ( check command buffer endSignal)
      //@deprecated ( check command buffer endSignal)
      //@deprecated ( check command buffer endSignal)
      VK_CHECK_RESULT(
          vkCreateFence( m_device->getDevice(), &fence_info, nullptr, &m_inFlightFences[ i ] ) );
    }
  }

  void VulkanRenderingDevice::waitIdle() noexcept {
    vkDeviceWaitIdle( m_device->getDevice() );
  }

  void VulkanRenderingDevice::recreateSwapChain() noexcept {
    vkDeviceWaitIdle( m_device->getDevice() );

    this->cleanupSwapChain();

    this->createSwapChain();
    this->createImageViews();
    this->createRenderPass();
    this->createGraphicsPipeline();
    this->createDepthResources();
    this->createFrameBuffers();
    this->createUniformBuffers();
    this->createDescriptorPool();
    this->createDescriptorSets();
    this->createCommandBuffers();
  }

  void VulkanRenderingDevice::cleanupSwapChain() noexcept {

    vkDestroyImageView( m_device->getDevice(), m_depthImageView, nullptr );
    vkDestroyImage( m_device->getDevice(), m_depthImage, nullptr );
    vkFreeMemory( m_device->getDevice(), m_depthImageMemory, nullptr );

    for ( auto &ubuffer : m_uniformBuffers ) {
      ubuffer.destory();
    }

    vkDestroyDescriptorPool( m_device->getDevice(), m_descriptorPool, nullptr );

    for ( const auto &framebuffer : m_swapChainFrameBuffers ) {
      vkDestroyFramebuffer( m_device->getDevice(), framebuffer, nullptr );
    }

    vkFreeCommandBuffers( m_device->getDevice(), m_commandPool,
                          static_cast<u32>( m_commandBuffers.size() ), m_commandBuffers.data() );

    vkDestroyPipeline( m_device->getDevice(), m_graphicsPipeline, nullptr );
    vkDestroyPipelineLayout( m_device->getDevice(), m_pipelineLayout, nullptr );
    vkDestroyRenderPass( m_device->getDevice(), m_renderPass, nullptr );

    for ( const auto &imageView : m_sawapChainImageViews ) {
      vkDestroyImageView( m_device->getDevice(), imageView, nullptr );
    }

    vkDestroySwapchainKHR( m_device->getDevice(), m_swapChain, nullptr );
  }

  void VulkanRenderingDevice::setFrameBufferResized( bool value ) noexcept {
    m_frambufferResized = value;
  }

  void VulkanRenderingDevice::createVertexBuffer() noexcept {

    VkDeviceSize buffer_size = sizeof( m_vertices[ 0 ] ) * m_vertices.size();

    VulkanBuffer staging_buffer;


    m_device->createBuffer( VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                                VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                            &staging_buffer, buffer_size, m_vertices.data() );


    m_device->createBuffer( VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
                            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &m_vertexBuffer, buffer_size );


    m_device->copyBuffer( &m_vertexBuffer, &staging_buffer, m_graphicsQueue );

    staging_buffer.destory();
  }

  void VulkanRenderingDevice::setVertexBuffer( std::vector<VulkanVertex> vertices ) noexcept {

    ASSERT_M( !vertices.empty(), "Verticies buffer cannot be empyt" );

    VkDeviceSize buffer_size = sizeof( vertices[ 0 ] ) * vertices.size();

    VulkanBuffer staging_buffer;

    m_device->createBuffer( VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                                VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                            &staging_buffer, buffer_size, vertices.data() );


    m_device->createBuffer( VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
                            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &m_vertexBuffer, buffer_size );


    m_device->copyBuffer( &m_vertexBuffer, &staging_buffer, m_graphicsQueue );

    staging_buffer.destory();
  }


  //@ deprecated
  //@ deprecated
  //@ deprecated
  //@ deprecated
  VkCommandBuffer VulkanRenderingDevice::beginSingleTimeCommands() noexcept {
    // @fix: we should create seperate command pool for this short-lived operations
    // we should use the VK_COMMAND_POOL_CREATE_TRANSFER_BIT
    VkCommandBufferAllocateInfo alloc_info = {};
    alloc_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    alloc_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    alloc_info.commandPool = m_commandPool;
    alloc_info.commandBufferCount = 1;

    VkCommandBuffer command_buffer;
    vkAllocateCommandBuffers( m_device->getDevice(), &alloc_info, &command_buffer );

    VkCommandBufferBeginInfo begin_info = {};
    begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    begin_info.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    vkBeginCommandBuffer( command_buffer, &begin_info );

    return command_buffer;
  }

  // @fix ( logic moved to VulkanDevice::flushCommandBuffer )
  // @fix ( logic moved to VulkanDevice::flushCommandBuffer )
  // @fix ( logic moved to VulkanDevice::flushCommandBuffer )
  void VulkanRenderingDevice::endSingleTimeCommands( VkCommandBuffer commandBuffer ) noexcept {

    vkEndCommandBuffer( commandBuffer );

    VkSubmitInfo submit_info = {};
    submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submit_info.commandBufferCount = 1;
    submit_info.pCommandBuffers = &commandBuffer;

    // we should query a separte queue for transfer operations
    vkQueueSubmit( m_graphicsQueue, 1, &submit_info, VK_NULL_HANDLE );
    vkQueueWaitIdle( m_graphicsQueue );

    vkFreeCommandBuffers( m_device->getDevice(), m_commandPool, 1, &commandBuffer );
  }

  void VulkanRenderingDevice::draw() noexcept {

    // m_submit_info.commandBufferCount = 1;
    // m_submit_info.pCommandBuffers = &m_commandBuffers[ m_imageIndex ];
    //
    // vkQueueSubmit( m_graphicsQueue, 1, &m_submit_info, VK_NULL_HANDLE );
  }


  void VulkanRenderingDevice::createIndexBuffer() noexcept {

    VkDeviceSize buffer_size = sizeof( m_indices[ 0 ] ) * m_indices.size();
    m_indicesSize = m_indices.size();

    VulkanBuffer staging_buffer;

    m_device->createBuffer( VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                                VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                            &staging_buffer, buffer_size, m_indices.data() );


    m_device->createBuffer( VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
                            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &m_indexBuffer, buffer_size );

    m_device->copyBuffer( &m_indexBuffer, &staging_buffer, m_graphicsQueue );

    staging_buffer.destory();
  }

  void VulkanRenderingDevice::setIndexBuffer( std::vector<u32> indices ) noexcept {

    ASSERT_M( !indices.empty(), "Indices buffer cannot be empty" );

    m_indicesSize = indices.size();
    VkDeviceSize buffer_size = sizeof( indices[ 0 ] ) * indices.size();

    VulkanBuffer staging_buffer;

    m_device->createBuffer( VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                                VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                            &staging_buffer, buffer_size, indices.data() );


    m_device->createBuffer( VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
                            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &m_indexBuffer, buffer_size );

    m_device->copyBuffer( &m_indexBuffer, &staging_buffer, m_graphicsQueue );

    staging_buffer.destory();
  }


  void VulkanRenderingDevice::createDescriptorSetLayout() noexcept {

    VkDescriptorSetLayoutBinding ubo_layout_binding = {};
    ubo_layout_binding.binding = 0;
    ubo_layout_binding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    ubo_layout_binding.descriptorCount = 1;
    // @notice: Which shader stage will be using the descriptor ? use
    // VK_SHADER_STAGE_ALL_GRAPHICS to use it everywhere
    ubo_layout_binding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
    ubo_layout_binding.pImmutableSamplers = nullptr;

    VkDescriptorSetLayoutBinding sampler_layout_binding = {};
    sampler_layout_binding.binding = 1;
    sampler_layout_binding.descriptorCount = 1;
    sampler_layout_binding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    sampler_layout_binding.pImmutableSamplers = nullptr;
    sampler_layout_binding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

    std::array<VkDescriptorSetLayoutBinding, 2> bindings = {ubo_layout_binding,
                                                            sampler_layout_binding};

    VkDescriptorSetLayoutCreateInfo layout_info = {};
    layout_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layout_info.bindingCount = static_cast<u32>( bindings.size() );
    layout_info.pBindings = bindings.data();

    VK_CHECK_RESULT( vkCreateDescriptorSetLayout( m_device->getDevice(), &layout_info, nullptr,
                                                  &m_descriptorSetLayout ) );
  }

  void VulkanRenderingDevice::createUniformBuffers() noexcept {

    VkDeviceSize buffer_size = sizeof( UniformBufferObject );

    m_uniformBuffers.resize( m_swapChainImages.size() );

    for ( auto &ubuffer : m_uniformBuffers ) {
      m_device->createBuffer( VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                              VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                                  VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                              &ubuffer, buffer_size );
    }
  }

  void VulkanRenderingDevice::updateUniformBuffer( u32 imageIndex ) noexcept {

    static auto start_time = std::chrono::high_resolution_clock::now();

    auto current_time = std::chrono::high_resolution_clock::now();
    f32 time = std::chrono::duration<f32, std::chrono::seconds::period>( current_time - start_time )
                   .count();

    UniformBufferObject ubo = {};
    ubo.model = glm::rotate( glm::mat4( 1.0f ), time * glm::radians( 90.0f ),
                             glm::vec3( 0.0f, 0.0f, 1.0f ) );

    ubo.view = glm::lookAt( glm::vec3( 2.0f, 2.0f, 2.0f ), glm::vec3( 0.0f, 0.0f, 0.0f ),
                            glm::vec3( 0.0f, 0.0f, 1.0f ) );

    ubo.proj = glm::perspective(
        glm::radians( 45.0f ),
        m_swapChainExtent.width / static_cast<f32>( m_swapChainExtent.height ), 0.1f, 10.0f );

    // GLM has the Y coordinate of the clip coordinates inverted.
    ubo.proj[ 1 ][ 1 ] *= -1;

    m_uniformBuffers[ imageIndex ].map( sizeof( ubo ) );
    m_uniformBuffers[ imageIndex ].copyTo( &ubo, sizeof( ubo ) );
    m_uniformBuffers[ imageIndex ].unmap();
  }

  void VulkanRenderingDevice::createDescriptorPool() noexcept {

    std::array<VkDescriptorPoolSize, 2> pool_sizes = {};
    pool_sizes[ 0 ].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    pool_sizes[ 0 ].descriptorCount = static_cast<u32>( m_swapChainImages.size() );
    pool_sizes[ 1 ].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    pool_sizes[ 1 ].descriptorCount = static_cast<u32>( m_swapChainImages.size() );

    VkDescriptorPoolCreateInfo pool_info = {};
    pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    pool_info.poolSizeCount = static_cast<u32>( pool_sizes.size() );
    pool_info.pPoolSizes = pool_sizes.data();
    pool_info.maxSets = static_cast<u32>( m_swapChainImages.size() );

    VK_CHECK_RESULT(
        vkCreateDescriptorPool( m_device->getDevice(), &pool_info, nullptr, &m_descriptorPool ) );
  }

  void VulkanRenderingDevice::createDescriptorSets() noexcept {

    std::vector<VkDescriptorSetLayout> layouts( m_swapChainImages.size(), m_descriptorSetLayout );

    VkDescriptorSetAllocateInfo alloc_info = {};
    alloc_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    alloc_info.descriptorPool = m_descriptorPool;
    alloc_info.descriptorSetCount = static_cast<uint32_t>( m_swapChainImages.size() );
    alloc_info.pSetLayouts = layouts.data();

    m_descriptorSets.resize( m_swapChainImages.size() );

    VK_CHECK_RESULT(
        vkAllocateDescriptorSets( m_device->getDevice(), &alloc_info, m_descriptorSets.data() ) );

    for ( size_t i = 0; i < m_swapChainImages.size(); i++ ) {

      VkDescriptorBufferInfo buffer_info = {};
      buffer_info.buffer = m_uniformBuffers[ i ].buffer;
      buffer_info.offset = 0;
      buffer_info.range = sizeof( UniformBufferObject );

      VkDescriptorImageInfo image_info = {};
      image_info.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
      image_info.imageView = m_textureImageView;
      image_info.sampler = m_textureSampler;

      std::array<VkWriteDescriptorSet, 2> descriptor_writes = {};
      descriptor_writes[ 0 ].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
      descriptor_writes[ 0 ].dstSet = m_descriptorSets[ i ];
      descriptor_writes[ 0 ].dstBinding = 0;
      descriptor_writes[ 0 ].dstArrayElement = 0;
      descriptor_writes[ 0 ].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
      descriptor_writes[ 0 ].descriptorCount = 1;
      descriptor_writes[ 0 ].pBufferInfo = &buffer_info;

      descriptor_writes[ 1 ].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
      descriptor_writes[ 1 ].dstSet = m_descriptorSets[ i ];
      descriptor_writes[ 1 ].dstBinding = 1;
      descriptor_writes[ 1 ].dstArrayElement = 0;
      descriptor_writes[ 1 ].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
      descriptor_writes[ 1 ].descriptorCount = 1;
      descriptor_writes[ 1 ].pImageInfo = &image_info;


      vkUpdateDescriptorSets( m_device->getDevice(), static_cast<u32>( descriptor_writes.size() ),
                              descriptor_writes.data(), 0, nullptr );
    }
  }

  void VulkanRenderingDevice::createTextureImage() noexcept {

    // @fixit: do we know that texture has loaded? should we handle somehow the error?
    auto texture = m_assetManager->loadAsset<Texture2D>(
        "vulkan_texture", FileSystem::getPath( "assets/textures/statue.jpg" ) );

    const auto tex_width = texture->getWidth();
    const auto tex_height = texture->getHeight();

    VkDeviceSize image_size = tex_width * tex_height * 4;

    VulkanBuffer staging_buffer;

    m_device->createBuffer( VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                                VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                            &staging_buffer, image_size, texture->getData() );


    createImage( tex_width, tex_height, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_TILING_OPTIMAL,
                 VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
                 VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_textureImage, m_textureImageMemory );

    transitionImageLayout( m_textureImage, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_UNDEFINED,
                           VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL );

    copyBufferToImage( staging_buffer.buffer, m_textureImage, tex_width, tex_height );

    transitionImageLayout( m_textureImage, VK_FORMAT_R8G8B8A8_SRGB,
                           VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                           VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL );

    staging_buffer.destory();
  }


  void VulkanRenderingDevice::createImage( u32 width, u32 height, VkFormat format,
                                           VkImageTiling tiling, VkImageUsageFlags flags,
                                           VkMemoryPropertyFlags properties, VkImage &image,
                                           VkDeviceMemory &imageMemory ) noexcept {

    VkImageCreateInfo image_info = {};
    image_info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    image_info.imageType = VK_IMAGE_TYPE_2D;
    image_info.extent.width = width;
    image_info.extent.height = height;
    image_info.extent.depth = 1;
    image_info.mipLevels = 1;
    image_info.arrayLayers = 1;
    image_info.format = format;
    image_info.tiling = tiling;
    image_info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    image_info.usage = flags;
    image_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    image_info.samples = VK_SAMPLE_COUNT_1_BIT;
    // We could use this for spare images ( e.g 3D textures for voxels terrain, we could use
    // this flag to avoid allocating memory to sotre large voloumes of "air" values.)
    image_info.flags = 0;

    VK_CHECK_RESULT( vkCreateImage( m_device->getDevice(), &image_info, nullptr, &image ) );

    VkMemoryRequirements mem_requirements;
    vkGetImageMemoryRequirements( m_device->getDevice(), image, &mem_requirements );

    VkMemoryAllocateInfo alloc_info = {};
    alloc_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    alloc_info.allocationSize = mem_requirements.size;
    alloc_info.memoryTypeIndex = m_device->getMemoryType( mem_requirements.memoryTypeBits,
                                                          VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT );

    VK_CHECK_RESULT(
        vkAllocateMemory( m_device->getDevice(), &alloc_info, nullptr, &imageMemory ) );

    vkBindImageMemory( m_device->getDevice(), image, imageMemory, 0 );
  }

  void VulkanRenderingDevice::transitionImageLayout( VkImage image, VkFormat format,
                                                     VkImageLayout oldLayout,
                                                     VkImageLayout newLayout ) noexcept {

    auto command_buffer = beginSingleTimeCommands();

    // Sync layout transitions  ( ensuring that a write to a buffer completes before
    // reading from it)
    VkImageMemoryBarrier barrier = {};
    barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    barrier.oldLayout = oldLayout;
    barrier.newLayout = newLayout;
    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.image = image;
    barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    barrier.subresourceRange.baseMipLevel = 0;
    barrier.subresourceRange.levelCount = 1;
    barrier.subresourceRange.baseArrayLayer = 0;
    barrier.subresourceRange.layerCount = 1;

    VkPipelineStageFlags source_stage;
    VkPipelineStageFlags destination_stage;

    if ( oldLayout == VK_IMAGE_LAYOUT_UNDEFINED &&
         newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL ) {

      barrier.srcAccessMask = 0;
      barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

      source_stage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
      destination_stage = VK_PIPELINE_STAGE_TRANSFER_BIT;
    } else if ( oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL &&
                newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL ) {

      barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
      barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

      source_stage = VK_PIPELINE_STAGE_TRANSFER_BIT;
      source_stage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
    } else if ( oldLayout == VK_IMAGE_LAYOUT_UNDEFINED &&
                newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL ) {
      barrier.srcAccessMask = 0;
      barrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT |
                              VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

      source_stage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
      destination_stage = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;

    } else {
      ASSERT_M( false, "Unsupported layout transition!" );
    }


    vkCmdPipelineBarrier( command_buffer, source_stage, destination_stage, 0, 0, nullptr, 0,
                          nullptr, 1, &barrier );


    endSingleTimeCommands( command_buffer );
  }

  void VulkanRenderingDevice::copyBufferToImage( VkBuffer buffer, VkImage image, u32 width,
                                                 u32 height ) noexcept {

    auto command_buffer = beginSingleTimeCommands();

    VkBufferImageCopy region = {};
    region.bufferOffset = 0;
    region.bufferRowLength = 0;
    region.bufferImageHeight = 0;

    region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    region.imageSubresource.mipLevel = 0;
    region.imageSubresource.baseArrayLayer = 0;
    region.imageSubresource.layerCount = 1;

    region.imageOffset = {0, 0, 0};
    region.imageExtent = {width, height, 1};

    vkCmdCopyBufferToImage( command_buffer, buffer, image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1,
                            &region );


    endSingleTimeCommands( command_buffer );
  }

  void VulkanRenderingDevice::createTextureImageView() noexcept {
    m_textureImageView =
        createImageView( m_textureImage, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_ASPECT_COLOR_BIT );
  }

  void VulkanRenderingDevice::createTextureSampler() noexcept {

    VkSamplerCreateInfo sampler_info = {};
    sampler_info.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    sampler_info.magFilter = VK_FILTER_LINEAR;
    sampler_info.minFilter = VK_FILTER_LINEAR;
    sampler_info.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    sampler_info.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    sampler_info.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    sampler_info.anisotropyEnable = VK_TRUE;
    sampler_info.maxAnisotropy = 16;
    sampler_info.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
    // @consider: unnormalizedCoordinates is used to spcify which coordinates type
    // we want to use, e.g normalized ( [0,1] range or unnormalized [0, texWidth] range ).
    // consider this for 2D apps and 3D apps
    sampler_info.unnormalizedCoordinates = VK_FALSE;
    // the above is usefull for shadow mapping
    sampler_info.compareEnable = VK_FALSE;
    sampler_info.compareOp = VK_COMPARE_OP_ALWAYS;
    sampler_info.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
    sampler_info.mipLodBias = 0.0f;
    sampler_info.minLod = 0.0f;
    sampler_info.maxLod = 0.0f;

    VK_CHECK_RESULT(
        vkCreateSampler( m_device->getDevice(), &sampler_info, nullptr, &m_textureSampler ) );
  }

  void VulkanRenderingDevice::createDepthResources() noexcept {

    VkFormat depth_format = findDepthFormat();

    createImage( m_swapChainExtent.width, m_swapChainExtent.height, depth_format,
                 VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
                 VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_depthImage, m_depthImageMemory );

    m_depthImageView = createImageView( m_depthImage, depth_format, VK_IMAGE_ASPECT_DEPTH_BIT );
  }

  VkFormat VulkanRenderingDevice::findSupportedFormat( const std::vector<VkFormat> &canditates,
                                                       VkImageTiling tiling,
                                                       VkFormatFeatureFlags features ) const
      noexcept {

    for ( VkFormat format : canditates ) {
      VkFormatProperties props;
      vkGetPhysicalDeviceFormatProperties( m_physicalDevice, format, &props );

      if ( tiling == VK_IMAGE_TILING_LINEAR &&
           ( props.linearTilingFeatures & features ) == features ) {
        return format;
      } else if ( tiling == VK_IMAGE_TILING_OPTIMAL &&
                  ( props.optimalTilingFeatures & features ) == features )
        return format;
    }

    ASSERT_M( false, "Failed to find supported format" );
  }

  VkFormat VulkanRenderingDevice::findDepthFormat() const noexcept {
    return findSupportedFormat(
        {VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT},
        VK_IMAGE_TILING_OPTIMAL, VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT );
  }

  bool VulkanRenderingDevice::hasSteniclComponent( VkFormat format ) noexcept {
    return format == VK_FORMAT_D32_SFLOAT_S8_UINT || format == VK_FORMAT_D24_UNORM_S8_UINT;
  }

  VkImageView VulkanRenderingDevice::createImageView( VkImage image, VkFormat format,
                                                      VkImageAspectFlags aspectFlags ) noexcept {

    VkImageViewCreateInfo create_info = {};
    create_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    create_info.image = image;
    create_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
    create_info.format = format;
    create_info.subresourceRange.aspectMask = aspectFlags;
    create_info.subresourceRange.baseMipLevel = 0;
    create_info.subresourceRange.levelCount = 1;
    create_info.subresourceRange.baseArrayLayer = 0;
    create_info.subresourceRange.layerCount = 1;

    VkImageView image_view;

    VK_CHECK_RESULT(
        vkCreateImageView( m_device->getDevice(), &create_info, nullptr, &image_view ) );

    return image_view;
  }


}    // namespace nile

