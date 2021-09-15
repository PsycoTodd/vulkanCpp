#include "VRenderer.h"
#include <algorithm>
#include <iostream>
#include <set>



VRenderer::VRenderer()
{
}


int VRenderer::init(GLFWwindow * newWindow)
{
  _window = newWindow;

  try {
    createInstance();
    createSurface();
    getPhysicalDevice();
    createLogicalDevice();
    createSwapChain();
  }
  catch (const std::runtime_error &e) {
    std::cout << "ERROR: " << e.what() << std::endl;
    return EXIT_FAILURE;
  }

  return 0;
}

void VRenderer::cleanup()
{
  for (auto image : _swapChainImages) {
    vkDestroyImageView(_mainDevice.logicalDevice, image.imageView, nullptr);
  }
  vkDestroySwapchainKHR(_mainDevice.logicalDevice, _swapChain, nullptr);
  vkDestroySurfaceKHR(_instance, _surface, nullptr);
  vkDestroyDevice(_mainDevice.logicalDevice, nullptr);
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

void VRenderer::createLogicalDevice()
{
  // get the queue family indices.
  QueueFamilyIndices indices = getQueueFamilies(_mainDevice.physicalDevice);

  // Vector for queue creation information, and set for family indices.
  std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
  std::set<int> queueFamilyIndices = { indices.graphicsFamily, indices.presentationFamily };


  // Queues that logical deivce needs to create
  for (int queueFamilyIndex : queueFamilyIndices) {
    VkDeviceQueueCreateInfo queueCreateInfo = {};
    queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queueCreateInfo.queueFamilyIndex = queueFamilyIndex;
    queueCreateInfo.queueCount = 1;
    float priority = 1.0f; // 1 is highest priority
    queueCreateInfo.pQueuePriorities = &priority;

    queueCreateInfos.push_back(queueCreateInfo);
  }
 
  // Information to create logical device
  VkDeviceCreateInfo deviceCreateInfo = {};
  deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
  deviceCreateInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
  deviceCreateInfo.pQueueCreateInfos = queueCreateInfos.data();
  deviceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
  deviceCreateInfo.ppEnabledExtensionNames = deviceExtensions.data();

  VkPhysicalDeviceFeatures deviceFeatures = {};


  deviceCreateInfo.pEnabledFeatures = &deviceFeatures; // Physical device feature that logical device will use.

  VkResult result = vkCreateDevice(_mainDevice.physicalDevice, &deviceCreateInfo, nullptr, &_mainDevice.logicalDevice);
  if (result != VK_SUCCESS) {
    throw std::runtime_error("Failed to create the deivce");
  }

  // We also want to get the queue.
  vkGetDeviceQueue(_mainDevice.logicalDevice, indices.graphicsFamily, 0, &_graphicsQueue);
  vkGetDeviceQueue(_mainDevice.logicalDevice, indices.presentationFamily, 0, &_presentationQueue);
}

void VRenderer::createSurface()
{
  // create surface (creating a surface crate info struct)
  VkResult result = glfwCreateWindowSurface(_instance, _window, nullptr, &_surface);
  if (result != VK_SUCCESS) {
    throw std::runtime_error("Failed to create a surface!");
  }
}

void VRenderer::createSwapChain()
{
  // Get swap chain details so we can pick the best settings.
  SwapChainDetails swapChainDetails = getSwapChainDetails(_mainDevice.physicalDevice);
  
  // 1. chose the best surface format.
  VkSurfaceFormatKHR surfaceFormat = chooseBestSurfaceFormat(swapChainDetails.formats);

  // 2. chose the best presentation mode.
  VkPresentModeKHR presentMode = chooseBestPresentationMode(swapChainDetails.presentationModes);

  // 3. choose the swap chain image resolution.
  VkExtent2D extent = chooseSwapExtent(swapChainDetails.surfaceCapabilities);

  // Decide how many image in the swap chain. Get one more than minimum to allow triple buffering.
  uint32_t imageCount = swapChainDetails.surfaceCapabilities.minImageCount + 1;

  if (swapChainDetails.surfaceCapabilities.maxImageCount > 0 && swapChainDetails.surfaceCapabilities.maxImageCount < imageCount) {
    imageCount = swapChainDetails.surfaceCapabilities.maxImageCount;
  }

  VkSwapchainCreateInfoKHR swapChainCreateInfo = {};
  swapChainCreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
  swapChainCreateInfo.imageFormat = surfaceFormat.format;
  swapChainCreateInfo.surface = _surface;
  swapChainCreateInfo.imageColorSpace = surfaceFormat.colorSpace;
  swapChainCreateInfo.presentMode = presentMode;
  swapChainCreateInfo.imageExtent = extent;
  swapChainCreateInfo.minImageCount = imageCount;
  swapChainCreateInfo.imageArrayLayers = 1; 
  swapChainCreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
  swapChainCreateInfo.preTransform = swapChainDetails.surfaceCapabilities.currentTransform;
  swapChainCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
  swapChainCreateInfo.clipped = VK_TRUE;

  // Get Queue family indices
  QueueFamilyIndices indices = getQueueFamilies(_mainDevice.physicalDevice);

  if (indices.graphicsFamily != indices.presentationFamily) {
    // Queue to share between.
    uint32_t queueFamilyIndices[] = {
      (uint32_t)indices.graphicsFamily,
      (uint32_t)indices.presentationFamily
    };

    swapChainCreateInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
    swapChainCreateInfo.queueFamilyIndexCount = 2;
    swapChainCreateInfo.pQueueFamilyIndices = queueFamilyIndices;
  }
  else {
    swapChainCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    swapChainCreateInfo.queueFamilyIndexCount = 0;
    swapChainCreateInfo.pQueueFamilyIndices = nullptr;
  }

  // If the swapchain been destroyed and this one repalces it. Then link old one to quickly hand over responsibilities.
  swapChainCreateInfo.oldSwapchain = VK_NULL_HANDLE;

  // Crate swapchain, finally.
  VkResult result = vkCreateSwapchainKHR(_mainDevice.logicalDevice, &swapChainCreateInfo, nullptr, &_swapChain);
  if (result != VK_SUCCESS) {
    throw std::runtime_error("Failed to create a swapchain!");
  }

  swapChainImageFormat = surfaceFormat.format;
  swapChainExtent = extent;

  uint32_t swapChainImageCount;
  vkGetSwapchainImagesKHR(_mainDevice.logicalDevice, _swapChain, &swapChainImageCount, nullptr);
  std::vector<VkImage> images(swapChainImageCount);
  vkGetSwapchainImagesKHR(_mainDevice.logicalDevice, _swapChain, &swapChainImageCount, images.data());
  
  for (VkImage image : images) {
    SwapchainImage swapChainImage = {};
    swapChainImage.image = image;
    swapChainImage.imageView = createImageView(swapChainImage.image, swapChainImageFormat, VK_IMAGE_ASPECT_COLOR_BIT);

    // swapChainImage list
    _swapChainImages.push_back(swapChainImage);
  }
}

void VRenderer::getPhysicalDevice()
{
  uint32_t deviceCount = 0;
  vkEnumeratePhysicalDevices(_instance, &deviceCount, nullptr);

  if (deviceCount == 0) {
    throw std::runtime_error("Can't find GPUs that support Vulkan instance!");
  }

  std::vector<VkPhysicalDevice> deviceVec(deviceCount);
  vkEnumeratePhysicalDevices(_instance, &deviceCount, deviceVec.data());

  // just return the first one.
  for (const auto &device : deviceVec) {
    if (checkDeviceSuitable(device)) {
      _mainDevice.physicalDevice = deviceVec[0];
    }
    
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

bool VRenderer::checkDeviceExtensionSupport(VkPhysicalDevice device)
{
  uint32_t extensionCount = 0;
  vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);
  
  if (extensionCount == 0) {
    return false;
  }

  std::vector<VkExtensionProperties> extensions(extensionCount);
  vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, extensions.data());
  // Check for the extensions.
  for (const auto &deviceExtension : deviceExtensions) {
    bool hasExt = false;
    for (const auto &extension : extensions) {
      if (strcmp(deviceExtension, extension.extensionName) == 0) {
        hasExt = true;
        break;
      }
    }
    if (!hasExt) {
      return false;
    }
  }
  return true;
}

bool VRenderer::checkDeviceSuitable(VkPhysicalDevice device)
{
  // Information about the device itself (ID, name, type).
  /*VkPhysicalDeviceProperties deviceProp;
  vkGetPhysicalDeviceProperties(device, &deviceProp);

  VkPhysicalDeviceFeatures deviceFeatures;
  VkPhysicalDeviceFeatures2*/

  QueueFamilyIndices indices = getQueueFamilies(device);

  bool extensionsSupported = checkDeviceExtensionSupport(device);

  bool swapChainValid = false;
  if (extensionsSupported) {
    SwapChainDetails swapChainDetails = getSwapChainDetails(device);
    swapChainValid = !swapChainDetails.presentationModes.empty() && !swapChainDetails.formats.empty();
  }
  return indices.isValid() && extensionsSupported && swapChainValid;

  return false;
}

QueueFamilyIndices VRenderer::getQueueFamilies(VkPhysicalDevice device)
{
  QueueFamilyIndices indices;
  uint32_t queueFamilyCount = 0;
  vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

  std::vector<VkQueueFamilyProperties> queueFamilyList(queueFamilyCount);
  vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilyList.data());

  int i = 0;
  for (const auto &queueFamily : queueFamilyList) {
    if (queueFamily.queueCount > 0 && queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)
    {
      indices.graphicsFamily = i; // If queue family is valid, then get index
    }

    // Check if Queue family supports presentation
    VkBool32 presentationSupport = false;
    vkGetPhysicalDeviceSurfaceSupportKHR(device, i, _surface, &presentationSupport);
    // Check if queue is presentation type (can be both graphics and presentation)
    if (queueFamily.queueCount > 0 && presentationSupport) {
      indices.presentationFamily = i;
    }

    if (indices.isValid()) {
      break;
    }

    i++;
  }

  return indices;
}

SwapChainDetails VRenderer::getSwapChainDetails(VkPhysicalDevice device)
{
  SwapChainDetails scdetails;

  // Get the surface capabilities for the given surface on the given physical devices
  vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, _surface, &scdetails.surfaceCapabilities);

  //-- FORMAT --
  uint32_t formatCount = 0;
  vkGetPhysicalDeviceSurfaceFormatsKHR(device, _surface, &formatCount, nullptr);

  if (formatCount) {
    scdetails.formats.resize(formatCount);
    vkGetPhysicalDeviceSurfaceFormatsKHR(device, _surface, &formatCount, scdetails.formats.data());
  }

  uint32_t presentationCount = 0;
  vkGetPhysicalDeviceSurfacePresentModesKHR(device, _surface, &presentationCount, nullptr);

  if (presentationCount) {
    scdetails.presentationModes.resize(presentationCount);
    vkGetPhysicalDeviceSurfacePresentModesKHR(device, _surface, &presentationCount, scdetails.presentationModes.data());
  }

  return scdetails;
}

VkSurfaceFormatKHR VRenderer::chooseBestSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& formats)
{
  if (formats.empty()) {
    throw std::runtime_error("Empty surface formats.");
  }
  // Format : VK_FORMAT_R8G8B8A8_UNORM
  // colorSpace : VK_COLOR_SPACE_SRGB_NONLINEAR_KHR

  if (formats.size() == 1 && formats.at(0).format == VK_FORMAT_UNDEFINED) {
    return{ VK_FORMAT_R8G8B8A8_UNORM, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR };
  }

  for (const auto &format : formats) {
    if ((format.format == VK_FORMAT_R8G8B8A8_UNORM ||
      format.format == VK_FORMAT_B8G8R8A8_UNORM) &&
      format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
      return format;
    }
  }

  return formats[0];
}

VkPresentModeKHR VRenderer::chooseBestPresentationMode(const std::vector<VkPresentModeKHR> presentationModes)
{
  if (presentationModes.empty()) {
    throw std::runtime_error("Presentation modes empty.");
  }
  for (const auto &presentationMode : presentationModes) {
    if (presentationMode == VK_PRESENT_MODE_MAILBOX_KHR) {
      return presentationMode;
    }
  }
  return VK_PRESENT_MODE_FIFO_KHR; // It is always available follow Vulkan spec.
}

VkExtent2D VRenderer::chooseSwapExtent(const VkSurfaceCapabilitiesKHR & surfaceCapabilities)
{
  if (surfaceCapabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()){
    return surfaceCapabilities.currentExtent;
  }
  else {
    int width, height;
    glfwGetFramebufferSize(_window, &width, &height);
    VkExtent2D newExtent = {};
    newExtent.width = static_cast<uint32_t>(width);
    newExtent.height = static_cast<uint32_t>(height);

    // Surface also define max and min, so make sure within boundaries by clamping value.
    newExtent.width = std::max(surfaceCapabilities.minImageExtent.width, std::min(surfaceCapabilities.maxImageExtent.width, newExtent.width));
    newExtent.height = std::max(surfaceCapabilities.minImageExtent.height, std::min(surfaceCapabilities.maxImageExtent.height, newExtent.height));
    return newExtent;
  }
}

VkImageView VRenderer::createImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags)
{
  VkImageViewCreateInfo viewCreateInfo = {};
  viewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
  viewCreateInfo.image = image;
  viewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
  viewCreateInfo.format = format;
  // Allows remap of rgba to other values.
  viewCreateInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
  viewCreateInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
  viewCreateInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY; 
  viewCreateInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

  // Subresources allow the view to view only a part of an image.
  viewCreateInfo.subresourceRange.aspectMask = aspectFlags;    // Which aspect of iamge to view (eg. COLOR_BIT for view color)
  viewCreateInfo.subresourceRange.baseMipLevel = 0;            // Start mipmap level to view from
  viewCreateInfo.subresourceRange.levelCount = 1;              // number of mipamp levels to view
  viewCreateInfo.subresourceRange.baseArrayLayer = 0;          // Start array level to view from
  viewCreateInfo.subresourceRange.layerCount = 1;              // Number of array levels to view

  // Create image view and return it.
  VkImageView imageView;
  VkResult ret = vkCreateImageView(_mainDevice.logicalDevice, &viewCreateInfo, nullptr, &imageView);
  if (ret != VK_SUCCESS) {
    throw std::runtime_error("Failed to create an image view.");
  }

  return imageView;
}
