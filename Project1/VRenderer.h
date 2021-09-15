#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <stdexcept>
#include <vector>

#include "Utilities.h"

class VRenderer
{
public:
  VRenderer();

  int init(GLFWwindow * newWindow);

  void cleanup();

  ~VRenderer();

private:
  GLFWwindow *_window;

  // Vulkan component
  VkInstance _instance;

  struct {
    VkPhysicalDevice physicalDevice;
    VkDevice logicalDevice;
  } _mainDevice;

  VkQueue _graphicsQueue;
  VkQueue _presentationQueue;
  VkSurfaceKHR _surface;
  VkSwapchainKHR _swapChain;
  std::vector<SwapchainImage> _swapChainImages;

  // Utility
  VkFormat swapChainImageFormat;
  VkExtent2D swapChainExtent;

  // vulkan function
  void createInstance();
  void createLogicalDevice();
  void createSurface();
  void createSwapChain();

  // get function
  void getPhysicalDevice();

  // Check if the hardware support all the extensions
  // -- checker functions.
  bool checkInstacneExtensionSupport(std::vector<const char*>& checkExtension);
  bool checkDeviceExtensionSupport(VkPhysicalDevice device);
  bool checkDeviceSuitable(VkPhysicalDevice device);


  // -- Getter functions
  QueueFamilyIndices getQueueFamilies(VkPhysicalDevice device);
  SwapChainDetails  getSwapChainDetails(VkPhysicalDevice device);

  // -- Chose functions
  VkSurfaceFormatKHR chooseBestSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& formats);
  VkPresentModeKHR chooseBestPresentationMode(const std::vector<VkPresentModeKHR> presentationModes);
  VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& surfaceCapabilities);

  // == Create functions
  VkImageView createImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags);
};

