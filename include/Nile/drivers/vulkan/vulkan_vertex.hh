#pragma once

#include <glm/glm.hpp>
#include <vulkan/vulkan.h>

#include <array>

namespace nile {

  struct VulkanVertex {

    glm::vec3 position;
    glm::vec3 color;
    glm::vec3 normal;
    glm::vec2 uv;

    static VkVertexInputBindingDescription getBindingDescription() {
      VkVertexInputBindingDescription binding_description = {};
      binding_description.binding = 0;
      binding_description.stride = sizeof( VulkanVertex );
      // VK_VERTEX_INPUT_RATE_VERTEX -> move to the next data entry after each vertex
      // VK_VERTEX_INPUT_RATE_INTANCE -> move the the next data entry after each instance
      // used for instanced rendering
      binding_description.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
      return binding_description;
    }
    static std::array<VkVertexInputAttributeDescription, 4> getAttributeDescriptions() {
      std::array<VkVertexInputAttributeDescription, 4> attribute_descriptions = {};

      attribute_descriptions[ 0 ].binding = 0;
      attribute_descriptions[ 0 ].location = 0;
      attribute_descriptions[ 0 ].format = VK_FORMAT_R32G32B32_SFLOAT;
      attribute_descriptions[ 0 ].offset = offsetof( VulkanVertex, position );

      attribute_descriptions[ 1 ].binding = 0;
      attribute_descriptions[ 1 ].location = 1;
      attribute_descriptions[ 1 ].format = VK_FORMAT_R32G32B32_SFLOAT;
      attribute_descriptions[ 1 ].offset = offsetof( VulkanVertex, color );

      attribute_descriptions[ 2 ].binding = 0;
      attribute_descriptions[ 2 ].location = 2;
      attribute_descriptions[ 2 ].format = VK_FORMAT_R32G32B32_SFLOAT;
      attribute_descriptions[ 2 ].offset = offsetof( VulkanVertex, normal );

      attribute_descriptions[ 3 ].binding = 0;
      attribute_descriptions[ 3 ].location = 3;
      attribute_descriptions[ 3 ].format = VK_FORMAT_R32G32_SFLOAT;
      attribute_descriptions[ 3 ].offset = offsetof( VulkanVertex, uv );

      return attribute_descriptions;
    }
  };
}    // namespace nile
