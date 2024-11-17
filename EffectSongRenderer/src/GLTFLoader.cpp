#include <GLTFLoader.h>
#define CGLTF_IMPLEMENTATION
#include "Renderer.h"
#include "Scene.h"
#include "cgltf.h"

namespace util {
bool loadGLTFData(std::string file_path) {
  Renderer* renderer = Renderer::getInstance();

  // 파싱 옵션 및 결과 데이터를 저장할 구조체
  cgltf_options options = {};
  cgltf_data* data = NULL;

  cgltf_result result = cgltf_parse_file(&options, file_path.c_str(), &data);
  if (result == cgltf_result_success) {
    result = cgltf_load_buffers(&options, data, file_path.c_str());
    if (result != cgltf_result_success) {
      return false;
    }
  } else {
    return false;
  }
  size_t offset = file_path.find_last_of('/');
  std::string gltfPath = file_path.substr(0, offset + 1);
  for (size_t i = 0; i < data->textures_count; i++) {
    renderer->addTexture(data->textures[i].image->uri,
                         new Texture(gltfPath, &data->textures[i]));
  }
  for (size_t i = 0; i < data->materials_count; i++) {
    renderer->addMaterial(data->materials[i].name,
                          new Material(&data->materials[i]));
  }

  for (size_t i = 0; i < data->scenes_count; i++) {
    Scene* scene = new Scene(data->scenes[i]);
    renderer->addScene(scene);
  }

  return true;
}

}  // namespace util