#pragma once

const std::vector<const char *> deviceExtensions = {
  VK_KHR_SWAPCHAIN_EXTENSION_NAME
};

struct QueueFamilyIndices {
  int graphicsFamily = 1; // location of the graphics queue family.
  int presentationFamily = -1;   // Location of Presentation Queue Family
  // Check if queue family is valid.
  bool isValid() {
    return graphicsFamily >= 0 && presentationFamily >= 0;
  }
};

struct SwapChainDetails {
  VkSurfaceCapabilitiesKHR surfaceCapabilities;
  std::vector<VkSurfaceFormatKHR> formats; // surface image formats.
  std::vector<VkPresentModeKHR> presentationModes;  // how image should be presented to screen.
};

struct SwapchainImage {
  VkImage image;
  VkImageView imageView;
};