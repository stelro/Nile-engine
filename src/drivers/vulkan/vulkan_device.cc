#include "Nile/drivers/vulkan/vulkan_device.hh"
#include "Nile/core/assert.hh"
#include "Nile/drivers/vulkan/vulkan_utils.hh"
#include "Nile/log/log.hh"

namespace nile {

  VulkanDevice::VulkanDevice( VkPhysicalDevice physicalDevice ) noexcept {
    ASSERT( physicalDevice );

    m_physicalDevice = physicalDevice;

    vkGetPhysicalDeviceFeatures( m_physicalDevice, &m_features );

    u32 queue_family_count {0};

    vkGetPhysicalDeviceQueueFamilyProperties( m_physicalDevice, &queue_family_count, nullptr );

    ASSERT( queue_family_count > 0 );
    m_queueFamilyProperties.resize( queue_family_count );
    vkGetPhysicalDeviceQueueFamilyProperties( m_physicalDevice, &queue_family_count,
                                              m_queueFamilyProperties.data() );
  }

  VulkanDevice::~VulkanDevice() noexcept {
    if ( m_logicalDevice ) {
      vkDestroyDevice( m_logicalDevice, nullptr );
    }
  }

  VulkanDevice::operator VkDevice() noexcept {
    return m_logicalDevice;
  }

  void VulkanDevice::createLogicalDevice( VkPhysicalDeviceFeatures enabledFeatures,
                                          std::vector<const char *> enabledExtensions,
                                          bool useSwapChain, VkQueueFlags queueFlagBits ) noexcept {

    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos {};

    const f32 default_queue_priority {1.0f};

    if ( queueFlagBits & VK_QUEUE_GRAPHICS_BIT ) {

      // This needs to be fixed
      m_queueFamilyIndices.graphics = getQueueFamilyIndex( VK_QUEUE_GRAPHICS_BIT );
      m_queueFamilyIndices.present = getQueueFamilyIndex( VK_QUEUE_GRAPHICS_BIT );

      VkDeviceQueueCreateInfo queue_info {};
      queue_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
      queue_info.queueFamilyIndex = m_queueFamilyIndices.graphics;
      queue_info.queueCount = 1;
      queue_info.pQueuePriorities = &default_queue_priority;

      queueCreateInfos.push_back( queue_info );

    } else {
      m_queueFamilyIndices.graphics = VK_NULL_HANDLE;
      m_queueFamilyIndices.present = VK_NULL_HANDLE;
    }

    if ( queueFlagBits & VK_QUEUE_COMPUTE_BIT ) {

      m_queueFamilyIndices.graphics = getQueueFamilyIndex( VK_QUEUE_COMPUTE_BIT );

      // If compute family index differs, we need an additional queue create info
      // for the compute queue
      if ( m_queueFamilyIndices.compute != m_queueFamilyIndices.graphics ) {

        VkDeviceQueueCreateInfo queue_info {};
        queue_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queue_info.queueFamilyIndex = m_queueFamilyIndices.compute;
        queue_info.queueCount = 1;
        queue_info.pQueuePriorities = &default_queue_priority;

        queueCreateInfos.push_back( queue_info );
      }

    } else {
      m_queueFamilyIndices.compute = VK_NULL_HANDLE;
    }

    if ( queueFlagBits & VK_QUEUE_TRANSFER_BIT ) {

      m_queueFamilyIndices.transfer = getQueueFamilyIndex( VK_QUEUE_TRANSFER_BIT );

      if ( ( m_queueFamilyIndices.transfer != m_queueFamilyIndices.graphics ) &&
           ( m_queueFamilyIndices.transfer != m_queueFamilyIndices.compute ) ) {

        VkDeviceQueueCreateInfo queue_info {};
        queue_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queue_info.queueFamilyIndex = m_queueFamilyIndices.transfer;
        queue_info.queueCount = 1;
        queue_info.pQueuePriorities = &default_queue_priority;

        queueCreateInfos.push_back( queue_info );
      }
    } else {
      m_queueFamilyIndices.transfer = VK_NULL_HANDLE;
    }

    // Create Logical Device repsentation
    std::vector<const char *> device_extensions( enabledExtensions );

    if ( useSwapChain ) {
      // If the device will be used for presenting to a display via a swapchain
      // we need to request the swapchain extension
      device_extensions.push_back( VK_KHR_SWAPCHAIN_EXTENSION_NAME );
    }

    m_enabledFeatures = enabledFeatures;

    VkDeviceCreateInfo device_create_info {};
    device_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    device_create_info.queueCreateInfoCount = static_cast<u32>( queueCreateInfos.size() );
    device_create_info.pQueueCreateInfos = queueCreateInfos.data();
    device_create_info.pEnabledFeatures = &m_enabledFeatures;

    if ( !device_extensions.empty() ) {
      device_create_info.enabledExtensionCount = static_cast<u32>( device_extensions.size() );
      device_create_info.ppEnabledExtensionNames = device_extensions.data();
    }

    if ( m_enableValidationLayers ) {
      device_create_info.enabledLayerCount = static_cast<u32>( m_validationLayers.size() );
      device_create_info.ppEnabledLayerNames = m_validationLayers.data();
    } else {
      device_create_info.enabledLayerCount = 0;
    }

    VK_CHECK_RESULT(
        vkCreateDevice( m_physicalDevice, &device_create_info, nullptr, &m_logicalDevice ) );
  }

  [[nodiscard]] u32 VulkanDevice::getQueueFamilyIndex( VkQueueFlagBits queueFlag ) const noexcept {

    // Find queue family that supports compute queue but not graphics
    if ( queueFlag & VK_QUEUE_COMPUTE_BIT ) {
      for ( usize i = 0; i < m_queueFamilyProperties.size(); i++ ) {
        if ( ( m_queueFamilyProperties[ i ].queueFlags & queueFlag ) &&
             ( ( m_queueFamilyProperties[ i ].queueFlags & VK_QUEUE_GRAPHICS_BIT ) == 0 ) ) {
          return i;
        }
      }
    }

    // Find queue family that supports only transfer queue but not
    // compute nor graphics queue
    if ( queueFlag & VK_QUEUE_TRANSFER_BIT ) {
      for ( usize i = 0; i < m_queueFamilyProperties.size(); i++ ) {
        if ( ( m_queueFamilyProperties[ i ].queueFlags & queueFlag ) &&
             ( ( m_queueFamilyProperties[ i ].queueFlags & VK_QUEUE_GRAPHICS_BIT ) == 0 ) &&
             ( m_queueFamilyProperties[ i ].queueFlags & VK_QUEUE_COMPUTE_BIT ) == 0 ) {
          return i;
        }
      }
    }

    // Other queue types, return one thhat support the requestd flag
    for ( usize i = 0; i < m_queueFamilyProperties.size(); i++ ) {
      if ( m_queueFamilyProperties[ i ].queueFlags & queueFlag ) {
        return i;
      }
    }

    log::fatal( "No queue family is supported by this GPU" );
    return 0;
  }

  VkResult VulkanDevice::createBuffer( VkBufferUsageFlags usageFlags,
                                       VkMemoryPropertyFlags memoryPropertyFlags,
                                       VulkanBuffer *buffer, VkDeviceSize size,
                                       void *data ) noexcept {

    buffer->device = m_logicalDevice;

    VkBufferCreateInfo buffer_info = {};
    buffer_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    buffer_info.size = size;
    buffer_info.usage = usageFlags;
    buffer_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    VK_CHECK_RESULT( vkCreateBuffer( m_logicalDevice, &buffer_info, nullptr, &buffer->buffer ) );

    VkMemoryRequirements mem_requirements;
    vkGetBufferMemoryRequirements( m_logicalDevice, buffer->buffer, &mem_requirements );

    VkMemoryAllocateInfo alloc_info = {};
    alloc_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    alloc_info.allocationSize = mem_requirements.size;
    alloc_info.memoryTypeIndex =
        getMemoryType( mem_requirements.memoryTypeBits, memoryPropertyFlags );

    // @warning: In reality we are not supposed to actually call vkAllocateMemory for every
    // individual buffer. The maximum number of simulatenous memory allocations is limited
    // by the maxMemoryAllocationsCount physical device limit. The right way to allocate
    // memory for a large number of objects at the same time is to create a custom allocator
    // that splits up a single allocation among many different objects by using offset
    // parameters.
    VK_CHECK_RESULT( vkAllocateMemory( m_logicalDevice, &alloc_info, nullptr, &buffer->memory ) );

    buffer->alignment = mem_requirements.alignment;
    buffer->size = size;
    buffer->usageFlags = usageFlags;
    buffer->memoryPropertyFlags = memoryPropertyFlags;

    // If a pointer to the buffer data has been passed, map the buffer and copy over the data
    if ( data != nullptr ) {

      VK_CHECK_RESULT( buffer->map() );
      memcpy( buffer->data, data, size );
      if ( ( memoryPropertyFlags & VK_MEMORY_PROPERTY_HOST_COHERENT_BIT ) == 0 ) {
        buffer->flush();
      }

      buffer->unmap();
    }

    buffer->setupDescriptor();

    // Attach the memory to the buffer object
    return buffer->bind();
  }

  u32 VulkanDevice::getMemoryType( u32 typeFilter, VkMemoryPropertyFlags memoryPropertyFlags ) const
      noexcept {

    VkPhysicalDeviceMemoryProperties mem_properties;
    vkGetPhysicalDeviceMemoryProperties( m_physicalDevice, &mem_properties );

    for ( u32 i = 0; i < mem_properties.memoryTypeCount; i++ ) {
      if ( ( typeFilter & ( 1 << i ) ) && ( mem_properties.memoryTypes[ i ].propertyFlags &
                                            memoryPropertyFlags ) == memoryPropertyFlags ) {
        return i;
      }
    }

    ASSERT_M( false, "Failed to find suitable memory type!" );
    return 0;
  }


}    // namespace nile
