#pragma once

#include "Nile/drivers/vulkan/vulkan_utils.hh"

#include <cstring>

namespace nile {

  struct VulkanBuffer {

    VkDevice device;
    VkBuffer buffer = VK_NULL_HANDLE;
    VkDeviceMemory memory = VK_NULL_HANDLE;
    VkDescriptorBufferInfo descriptor;
    VkDeviceSize size = 0;
    VkDeviceSize alignment = 0;
    void *data = nullptr;

    // @brief: Usage flags to be filled by external source at buffer creation
    // used to query at some later point
    VkBufferUsageFlags usageFlags;

    // @brief: memory property flags to be filled by external source at buffer creation
    // used to query at some later point
    VkMemoryPropertyFlags memoryPropertyFlags;

    VkResult map( VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0 ) noexcept {
      return vkMapMemory( device, memory, offset, size, 0, &data );
    }

    void unmap() noexcept {
      if ( data ) {
        vkUnmapMemory( device, memory );
        data = nullptr;
      }
    }

    VkResult bind( VkDeviceSize offset = 0 ) noexcept {
      return vkBindBufferMemory( device, buffer, memory, offset );
    }

    void copyTo( void *t_data, VkDeviceSize size ) noexcept {
      ASSERT( data );
      memcpy( data, t_data, size );
    }

    void setupDescriptor( VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0 ) {
      descriptor.offset = offset;
      descriptor.buffer = buffer;
      descriptor.range = size;
    }

    VkResult flush( VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0 ) {
      // Flush a memory range of the buffer to make it visible to the device
      // only required for non-coherent memory
      VkMappedMemoryRange mappedRange = {};
      mappedRange.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
      mappedRange.memory = memory;
      mappedRange.offset = offset;
      mappedRange.size = size;

      return vkFlushMappedMemoryRanges( device, 1, &mappedRange );
    }

    VkResult invalidate( VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0 ) {
      // Invalidate a memory range of the buffer to make it visible to the host
      // only required for non-coherent memory
      VkMappedMemoryRange mappedRange = {};
      mappedRange.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
      mappedRange.memory = memory;
      mappedRange.offset = offset;
      mappedRange.size = size;

      return vkInvalidateMappedMemoryRanges( device, 1, &mappedRange );
    }

    void destory() noexcept {

      if ( buffer ) {
        vkDestroyBuffer( device, buffer, nullptr );
      }

      if ( memory ) {
        vkFreeMemory( device, memory, nullptr );
      }
    }
  };

}    // namespace nile
