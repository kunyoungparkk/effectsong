#pragma once
#define _CRT_SECURE_NO_WARNINGS
#define GLM_ENABLE_EXPERIMENTAL
#define FIXED_DELTATIME (1.0f/60.0f)
#ifdef _WIN64
#include "GL/glew.h"
#elif defined(__EMSCRIPTEN__)

#endif
#include "cgltf.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/quaternion.hpp>
#include <vector>
#include <iostream>
#include <list>