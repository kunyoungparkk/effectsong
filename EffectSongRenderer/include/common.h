#pragma once
#define _CRT_SECURE_NO_WARNINGS
#define GLM_ENABLE_EXPERIMENTAL
#define TARGET_FPS (60)
#ifdef _WIN64
#include "GL/glew.h"
#define EFFECTSONG_ROOT "../../"
#elif defined(__EMSCRIPTEN__)
#include "GLES3/gl3.h"
#define EFFECTSONG_ROOT "../"
#endif
#include "cgltf.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/quaternion.hpp>
#include <vector>
#include <iostream>
#include <list>
#include <string>
