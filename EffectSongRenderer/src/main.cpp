#include "common.h"
#include <SDL.h>
#include <Windows.h>
#include <iostream>
#include <vector>
#include "GLTFLoader.h"
#include "Renderer.h"
#include "Node.h"
#include "Scene.h"

int main(int argc, char* argv[]) {
	SDL_Window* window = NULL;

	// SDL 초기화
	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		printf("SDL Initialization Fail: %s\n", SDL_GetError());
		return 1;
	}

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
	SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4);

	// 윈도우 창 생성
	window =
		SDL_CreateWindow("SDL2 With OpenGL", SDL_WINDOWPOS_UNDEFINED,
			SDL_WINDOWPOS_UNDEFINED, 640, 480, SDL_WINDOW_OPENGL);
	// OpenGL 컨텍스트 생성
	SDL_GLContext context = SDL_GL_CreateContext(window);

	Renderer* renderer = Renderer::getInstance();

	//util::loadGLTFData("../../res/2.0/DamagedHelmet/glTF/DamagedHelmet.gltf");
	//util::loadGLTFData("../../res/2.0/Fox/glTF/Fox.gltf");
	util::loadGLTFData("../../res/2.0/Duck/glTF/Duck.gltf");
	//util::loadGLTFData("../../res/2.0/Box/glTF/Box.gltf");
	//util::loadGLTFData("../../res/2.0/ToyCar/glTF/ToyCar.gltf");
	//util::loadGLTFData("../../res/2.0/WaterBottle/glTF/WaterBottle.gltf");
	//util::loadGLTFData("../../res/2.0/Lantern/glTF/Lantern.gltf");
	//util::loadGLTFData("../../res/2.0/Suzanne/glTF/Suzanne.gltf");
	//util::loadGLTFData("../../res/2.0/NewFolder/untitled.gltf");
	//util::loadGLTFData("../../res/2.0/NewNewFolder/untitled.gltf");
	//util::loadGLTFData("../../res/car_gltf/ioniq.gltf");

	SDL_Event event;
	bool running = true;

	// camera control
	float cam_speed = 2.0f;
	uint32_t last_time = SDL_GetTicks();
	float delta_time = 0.0f;
	bool b_mouse_pressed = false;
	int32_t prev_mouse_coord[2] = { 0, };
	float cam_xRot = 0.0f;
	float cam_yRot = 180.0f;

	while (running) {
		uint32_t current_time = SDL_GetTicks();
		delta_time = (current_time - last_time) / 1000.0f;
		last_time = current_time;

		const uint8_t* state = SDL_GetKeyboardState(nullptr);
		Node* activeCam = renderer->getActiveScene()->getActiveCamera()->getNode();
		if (state[SDL_SCANCODE_W]) {
			activeCam->setPosition(activeCam->getPosition() +
				-1.0f * cam_speed * delta_time *
				activeCam->getFront());
		}
		if (state[SDL_SCANCODE_A]) {
			activeCam->setPosition(activeCam->getPosition() +
				-1.0f * cam_speed * delta_time *
				activeCam->getLeft());
		}
		if (state[SDL_SCANCODE_S]) {
			activeCam->setPosition(activeCam->getPosition() +
				cam_speed * delta_time * activeCam->getFront());
		}
		if (state[SDL_SCANCODE_D]) {
			activeCam->setPosition(activeCam->getPosition() +
				cam_speed * delta_time * activeCam->getLeft());
		}
		if (state[SDL_SCANCODE_E]) {
			activeCam->setPosition(activeCam->getPosition() +
				cam_speed * delta_time * glm::vec3(0, 1, 0));
		}
		if (state[SDL_SCANCODE_Q]) {
			activeCam->setPosition(activeCam->getPosition() +
				cam_speed * delta_time * glm::vec3(0, -1, 0));
		}
		while (SDL_PollEvent(&event)) {
			switch (event.type) {
			case SDL_KEYDOWN:
				break;
			case SDL_MOUSEBUTTONDOWN:
				b_mouse_pressed = true;
				prev_mouse_coord[0] = event.button.x;
				prev_mouse_coord[1] = event.button.y;
				break;
			case SDL_MOUSEMOTION:
				if (b_mouse_pressed) {
					cam_xRot += event.button.y - prev_mouse_coord[1];
					//TODO: 45도쯤 넘어가면 이상하게 흔들리는 현상 해결
					cam_xRot = glm::clamp(cam_xRot, -40.0f, 40.0f);
					cam_yRot -= event.button.x - prev_mouse_coord[0];
					activeCam->setRotation(glm::angleAxis(glm::radians(cam_xRot),
						-1.0f * activeCam->getLeft()) *
						glm::angleAxis(glm::radians(cam_yRot),
							glm::vec3(0.0f, 1.0f, 0.0f)));
					prev_mouse_coord[0] = event.button.x;
					prev_mouse_coord[1] = event.button.y;
				}
				break;
			case SDL_MOUSEBUTTONUP:
				b_mouse_pressed = false;
				break;
			case SDL_QUIT:
				running = false;
				break;
			}
		}
		renderer->update();
		// render
		renderer->render();
		SDL_GL_SwapWindow(window);
	}

	// 종료
	SDL_GL_DeleteContext(context);

	SDL_DestroyWindow(window);
	SDL_Quit();
	return 0;
}
