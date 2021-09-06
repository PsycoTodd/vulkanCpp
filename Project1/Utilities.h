#pragma once

struct QueueFamilyIndices {
  int graphicsFamily = 1; // location of the graphics queue family.
  
  // Check if queue family is valid.
  bool isValid() {
    return graphicsFamily >= 0;
  }
};