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

  // vulkan function
  void createInstance();
  void createLogicalDevice();

  // get function
  void getPhysicalDevice();

  // Check if the hardware support all the extensions
  // -- checker functions.
  bool checkInstacneExtensionSupport(std::vector<const char*>& checkExtension);
  bool checkDeviceSuitable(VkPhysicalDevice device);


  // -- Getter functions
  QueueFamilyIndices getQueueFamilies(VkPhysicalDevice device);

};

