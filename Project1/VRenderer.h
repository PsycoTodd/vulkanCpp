#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <stdexcept>
#include <vector>

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

  // vulkan function
  void createInstance();

  // Check if the hardware support all the extensions
  bool checkInstacneExtensionSupport(std::vector<const char*>& checkExtension);

};

