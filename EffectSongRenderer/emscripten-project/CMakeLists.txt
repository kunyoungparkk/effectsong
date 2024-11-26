# 요구 CMake 최소 버전
cmake_minimum_required(VERSION 3.19)

# 프로젝트 이름 및 버전
project("effectsong-core")
set(PROJECT_VERSION_MAJOR 0)
set(PROJECT_VERSION_MINOR 1)
set(PROJECT_VERSION_PATCH 0)
set(PROJECT_ROOT ${CMAKE_SOURCE_DIR}/..)

# 빌드 형상(Configuration)
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
  message(STATUS
    "CMAKE_BUILD_TYPE is not entered, default to ${CMAKE_BUILD_TYPE}.")
else()
  if(NOT ((CMAKE_BUILD_TYPE STREQUAL "Debug") OR
          (CMAKE_BUILD_TYPE STREQUAL "Release")))
    message(FATAL_ERROR
      "CMAKE_BUILD_TYPE is not valid. Valid values are Debug or Release.")
  endif()
endif()

# 빌드 대상 파일명
set(WASM_OUTPUT_NAME  "effectsong-core")

# 파일 출력 경로 : 바로 Editor로 넘기기
#set(OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/build/${CMAKE_BUILD_TYPE})
set(OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/../../EffectSongEditor/public)

set(EFFECTSONG_INCLUDE
  ${PROJECT_ROOT}/include
  ${PROJECT_ROOT}/thirdparty
  ${PROJECT_ROOT}/thirdparty/cgltf
  ${PROJECT_ROOT}/thirdparty/dr_libs
  ${PROJECT_ROOT}/thirdparty/fftw3/include
  ${PROJECT_ROOT}/thirdparty/stb-master
)

# 빌드 대상 라이브러리 소스 파일 목록
set(EFFECTSONG_LIBRARY_SOURCE_FILES
   ${PROJECT_ROOT}/src/ArtShader.cpp
   ${PROJECT_ROOT}/src/Camera.cpp
   ${PROJECT_ROOT}/src/GLTFLoader.cpp
   ${PROJECT_ROOT}/src/IBLPrimitive.cpp
   ${PROJECT_ROOT}/src/Light.cpp
   ${PROJECT_ROOT}/src/main.cpp
   ${PROJECT_ROOT}/src/Material.cpp
   ${PROJECT_ROOT}/src/Node.cpp
   ${PROJECT_ROOT}/src/Primitive.cpp
   ${PROJECT_ROOT}/src/Renderer.cpp
   ${PROJECT_ROOT}/src/Scene.cpp
   ${PROJECT_ROOT}/src/SoundTexture.cpp
   ${PROJECT_ROOT}/src/Texture.cpp
)

set(COPTS "${COPTS} -DPLATFORM_DEPENDENCY=_WASM")
set(COPTS "${COPTS} -sUSE_SDL=2")
# set(COPTS "${COPTS} -sMEMORY64=1")

# emcc compile options
set(LOPTS "${LOPTS} -sUSE_SDL=2")
set(LOPTS "${LOPTS} -lembind")
set(LOPTS "${LOPTS} -sFULL_ES3")
set(LOPTS "${LOPTS} -s USE_WEBGL2=1")
set(LOPTS "${LOPTS} -sMAX_WEBGL_VERSION=2")
set(LOPTS "${LOPTS} -sMIN_WEBGL_VERSION=2")
set(LOPTS "${LOPTS} -s WASM=1")
set(LOPTS "${LOPTS} -s ENVIRONMENT='web'")
set(LOPTS "${LOPTS} -s MAXIMUM_MEMORY=4GB")
set(LOPTS "${LOPTS} -s ALLOW_MEMORY_GROWTH=1")
set(LOPTS "${LOPTS} -s STACK_SIZE=1048576")
set(LOPTS "${LOPTS} -s GL_MAX_TEMP_BUFFER_SIZE=8388608")
set(LOPTS "${LOPTS} -s EXPORTED_RUNTIME_METHODS=['addFunction','removeFunction','FS']")
set(LOPTS "${LOPTS} -s ALLOW_TABLE_GROWTH=1")
set(LOPTS "${LOPTS} -s MODULARIZE=1")
set(LOPTS "${LOPTS} -s EXPORT_NAME='createModule'")
set(LOPTS "${LOPTS} -std=c++11")
set(LOPTS "${LOPTS} --preload-file ../res/IBL")
set(LOPTS "${LOPTS} --preload-file ../res/music")

set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${LOPTS}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${COPTS}")

# "Debug" 형상 한정 컴파일 옵션, 링크 옵션
set(CMAKE_CXX_FLAGS_DEBUG "-g")

# "Release" 형상 한정 컴파일 옵션, 링크 옵션
set(CMAKE_CXX_FLAGS_RELEASE "-O3 --llvm-lto 1" )

# 출력 디렉토리
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${OUTPUT_DIRECTORY})

# 라이브러리 소스 파일들을 exe(js)로 추가
add_executable(
  ${WASM_OUTPUT_NAME}
  ${EFFECTSONG_LIBRARY_SOURCE_FILES}
)

set_target_properties(${WASM_OUTPUT_NAME}
  PROPERTIES
  SUFFIX ".js"
)

target_include_directories(
  ${WASM_OUTPUT_NAME}
  PRIVATE
  ${EFFECTSONG_INCLUDE}
)

target_link_libraries(
  ${WASM_OUTPUT_NAME} 
  PRIVATE
  ${PROJECT_ROOT}/thirdparty/fftw3/lib/libfftw3f.a
)