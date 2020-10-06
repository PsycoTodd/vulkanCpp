#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <stdexcept>
#include <vector>
#include "VRenderer.h"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/mat4x4.hpp>
#include <glm/ext.hpp>

#include <iostream>

GLFWwindow * window;
VRenderer vulkanRenderer;


void initWindow(std::string wName= "Test Window", const int width = 800, const int height = 600)
{
  // initialize glfw.
  glfwInit();

  // set gltf to not work with opengl.
  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
  glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE); // resize for vulkan would be complex
  window = glfwCreateWindow(width, height, wName.c_str(), nullptr, nullptr);

}

int main()
{
  // Create window
  initWindow("Test Todd", 800, 600);

  // create the vulkan renderer instance
  if (vulkanRenderer.init(window)) {
    return EXIT_FAILURE;
  }

  // loop until close
  while (!glfwWindowShouldClose(window)) {
    glfwPollEvents();
  }
  
  // destroy
  vulkanRenderer.cleanup();
  glfwDestroyWindow(window);
  glfwTerminate();
}

