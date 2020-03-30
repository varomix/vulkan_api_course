#ifndef VALIDATION_H
#define VALIDATION_H

#include <vector>

bool validationEnabled = true;

const std::vector<const char*> validationLayers = {
    "VK_LAYER_LUNARG_standard_validation"
};

#endif // VALIDATION_H
