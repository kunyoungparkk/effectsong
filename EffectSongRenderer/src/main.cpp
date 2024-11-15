#include "common.h"
#include <SDL.h>
#include <Windows.h>
#include <iostream>

#include "GLTFLoader.h"
#include "Renderer.h"
#include "Node.h"
#include "Scene.h"

int main(int argc, char* argv[]) {
	SDL_Window* window = NULL;
	int initWidth = 1000;
	int initHeight = 800;
	// SDL �ʱ�ȭ
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

	// ������ â ����
	window =
		SDL_CreateWindow("SDL2 With OpenGL", SDL_WINDOWPOS_UNDEFINED,
			SDL_WINDOWPOS_UNDEFINED, initWidth, initHeight, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
	// OpenGL ���ؽ�Ʈ ����
	SDL_GLContext context = SDL_GL_CreateContext(window);

	Renderer* renderer = Renderer::getInstance();
	renderer->setWidth(initWidth);
	renderer->setHeight(initHeight);

	//util::loadGLTFData("../../res/2.0/DamagedHelmet/glTF/DamagedHelmet.gltf");
	//util::loadGLTFData("../../res/2.0/Fox/glTF/Fox.gltf");
	//util::loadGLTFData("../../res/2.0/Duck/glTF/Duck.gltf");
	//util::loadGLTFData("../../res/2.0/Box/glTF/Box.gltf");
	//util::loadGLTFData("../../res/2.0/ToyCar/glTF/ToyCar.gltf");
	//util::loadGLTFData("../../res/2.0/WaterBottle/glTF/WaterBottle.gltf");
	//util::loadGLTFData("../../res/2.0/Lantern/glTF/Lantern.gltf");
	//util::loadGLTFData("../../res/2.0/Suzanne/glTF/Suzanne.gltf");
	util::loadGLTFData("../../res/2.0/NewFolder/untitled.gltf");
	//util::loadGLTFData("../../res/2.0/NewNewFolder/untitled.gltf");
	//util::loadGLTFData("../../res/car_gltf/ioniq.gltf");

	SDL_Event event;
	bool running = true;

	// camera control
	float cam_speed = 2.0f;
	bool b_mouse_pressed = false;
	int32_t prev_mouse_coord[2] = { 0, };
	float cam_xRot = 0.0f;
	float cam_yRot = 180.0f;
	uint32_t frameStart;
	uint32_t frameTime;

	float musicTime = 0.0f;
	while (running) {
		frameStart = SDL_GetTicks();

		const uint8_t* state = SDL_GetKeyboardState(nullptr);
		Node* activeCam = renderer->getActiveScene()->getActiveCamera()->getNode();
		if (state[SDL_SCANCODE_W]) {
			activeCam->setPosition(activeCam->getPosition() +
				-1.0f * cam_speed * FIXED_DELTATIME *
				activeCam->getFront());
		}
		if (state[SDL_SCANCODE_A]) {
			activeCam->setPosition(activeCam->getPosition() +
				-1.0f * cam_speed * FIXED_DELTATIME *
				activeCam->getLeft());
		}
		if (state[SDL_SCANCODE_S]) {
			activeCam->setPosition(activeCam->getPosition() +
				cam_speed * FIXED_DELTATIME * activeCam->getFront());
		}
		if (state[SDL_SCANCODE_D]) {
			activeCam->setPosition(activeCam->getPosition() +
				cam_speed * FIXED_DELTATIME * activeCam->getLeft());
		}
		if (state[SDL_SCANCODE_E]) {
			activeCam->setPosition(activeCam->getPosition() +
				cam_speed * FIXED_DELTATIME * glm::vec3(0, 1, 0));
		}
		if (state[SDL_SCANCODE_Q]) {
			activeCam->setPosition(activeCam->getPosition() +
				cam_speed * FIXED_DELTATIME * glm::vec3(0, -1, 0));
		}
		while (SDL_PollEvent(&event)) {
			switch (event.type) {
			case SDL_KEYDOWN:
				break;
			case SDL_WINDOWEVENT_RESIZED:
				renderer->setWidth(event.window.data1);
				renderer->setHeight(event.window.data2);
				break;
			case SDL_MOUSEBUTTONDOWN:
				b_mouse_pressed = true;
				prev_mouse_coord[0] = event.button.x;
				prev_mouse_coord[1] = event.button.y;
				break;
			case SDL_MOUSEMOTION:
				if (b_mouse_pressed) {
					cam_xRot += event.button.y - prev_mouse_coord[1];
					//TODO: 45���� �Ѿ�� �̻��ϰ� ��鸮�� ���� �ذ�
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
		renderer->update(musicTime);
		// render
		renderer->render();
		SDL_GL_SwapWindow(window);
		
		frameTime = SDL_GetTicks() - frameStart;
		//fixed deltatime
		if (FIXED_DELTATIME * 1000.0 > frameTime)
		{
			SDL_Delay(FIXED_DELTATIME * 1000.0 - frameTime);
		}
		musicTime += FIXED_DELTATIME;
	}

	// ����
	SDL_GL_DeleteContext(context);

	SDL_DestroyWindow(window);
	SDL_Quit();
	return 0;
}
