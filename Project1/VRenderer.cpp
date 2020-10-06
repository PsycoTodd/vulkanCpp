#include "VRenderer.h"
#include <iostream>



VRenderer::VRenderer()
{
}


int VRenderer::init(GLFWwindow * newWindow)
{
  _window = newWindow;

  try {
    createInstance();
  }
  catch (const std::runtime_error &e) {
    std::cout << "ERROR: " << e.what() << std::endl;
    return EXIT_FAILURE;
  }

  return 0;
}

void VRenderer::cleanup()
{
  vkDestroyInstance(_instance, nullptr);
}

VRenderer::~VRenderer()
{
}

void VRenderer::createInstance()
{
  // Info about the application.
  // most data here doesn't affect the program, it is for developer.
  VkApplicationInfo appInfo = {};
  appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
  appInfo.pApplicationName = "vulkan App";
  appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
  appInfo.pEngineName = "No Engine";
  appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
  appInfo.apiVersion = VK_API_VERSION_1_0;


  // creation information for a vk instance.
  VkInstanceCreateInfo createInfo = {};
  createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
  createInfo.pApplicationInfo = &appInfo;

  // Create a list to hold instance extensions.
  std::vector<const char*> instanceExtensions = std::vector<const char*>();

  // set up extensions instance will use.
  uint32_t glfwExtensionCount = 0;
  const char** glfwExtensions;
  glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
  // add glfw extenstions
  for (size_t i = 0; i < glfwExtensionCount; ++i) {
    instanceExtensions.push_back(glfwExtensions[i]);
  }

  if (!checkInstacneExtensionSupport(instanceExtensions)) {
    throw std::runtime_error("VkInstance does not provided glfw needed extensions.");
  }

  createInfo.enabledExtensionCount = static_cast<uint32_t>(instanceExtensions.size());
  createInfo.ppEnabledExtensionNames = instanceExtensions.data();

  //TODO
  createInfo.enabledLayerCount = 0;
  createInfo.ppEnabledLayerNames = nullptr;

  // create instance
  VkResult result = vkCreateInstance(&createInfo, nullptr, &_instance);

  if (result != VK_SUCCESS) {
    throw std::runtime_error("Failed to create a vulkan instance.");
  }
}

bool VRenderer::checkInstacneExtensionSupport(std::vector<const char*>& checkExtension)
{
  // need to get number of extensions to create array.
  uint32_t extensionCount = 0;
  vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
  
  std::vector<VkExtensionProperties> extensions(extensionCount);
  vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data());

  // check if the given extensions are in the list of available extensions.
  for (const auto &checkExtension : checkExtension) {
    bool hasExtension = false;
    for (const auto &extension : extensions) {
      if (strcmp(extension.extensionName, checkExtension)) {
        hasExtension = true;
        break;
      }
    }

    if (!hasExtension) {
      return false;
    }
  }
  return true;
}
