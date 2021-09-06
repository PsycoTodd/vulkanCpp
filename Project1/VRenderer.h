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

  // vulkan function
  void createInstance();
  void createLogicalDevice();
  void createSurface();

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

};

