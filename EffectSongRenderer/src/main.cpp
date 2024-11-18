#include "common.h"
#include <SDL.h>
#include <iostream>
#include "GLTFLoader.h"
#include "Renderer.h"
#include "Node.h"
#include "Scene.h"
#include "ArtShader.h"

SDL_GLContext g_context;
SDL_Window* g_window;

void initialize(int width, int height);
void deInitialize();
void loop();

#ifdef _WIN64
float musicTime = 0.0f;
int initialWidth = 1000;
int initialHeight = 800;

void windowTestProc()
{
	util::loadGLTFData("../../res/2.0/DamagedHelmet/glTF/DamagedHelmet.gltf");
	util::loadGLTFData("../../res/2.0/Duck/glTF/Duck.gltf");
	Renderer::getInstance()->getSceneAt(1)->getNodeAt(0)->setPosition(glm::vec3(1.0f, 0.0f, 0.0f));
	//Renderer::getInstance()->setBackgroundColor(glm::vec4(0.5f, 0.5f, 0.0f, 1.0f));

	ArtShader::getInstance()->setVertexCount(10000);
	ArtShader::getInstance()->setPrimitiveMode(GL_POINTS);
	ArtShader::getInstance()->setVertexShader(R"(
#define PI 3.14159
#define NUM_SEGMENTS 51.0
#define NUM_POINTS (NUM_SEGMENTS * 2.0)
#define STEP 5.0

vec3 hsv2rgb(vec3 c) {
  c = vec3(c.x, clamp(c.yz, 0.0, 1.0));
  vec4 K = vec4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);
  vec3 p = abs(fract(c.xxx + K.xyz) * 6.0 - K.www);
  return c.z * mix(K.xxx, clamp(p - K.xxx, 0.0, 1.0), c.y);
}

void main() {
  float point = mod(floor(vertexId / 2.0) + mod(vertexId, 2.0) * STEP, NUM_SEGMENTS);
  float count = floor(vertexId / NUM_POINTS);
  float offset = count * 0.02;
  float angle = point * PI * 2.0 / NUM_SEGMENTS - offset;
//  float rPulse = pow(sin(time * 3. + -count * 0.05), 4.0);
  float spread = 0.01;
  float off = 0.1;
  float speed  = count * 0.004;
  float snd = (
     texture(sound, vec2(off + spread * 0., speed)).r +
     texture(sound, vec2(off + spread * 1., speed)).r + 
     texture(sound, vec2(off + spread * 2., speed)).r) / 2.;
//  radius = radius + pow(snd, 5.0) * 0.1;
//  float radius = count * 0.02 + rPlus * 0.2 + 0.0;
  float rPulse = pow(snd, 5.0);
  float radius = count * 0.02 + rPulse * 0.2;
  float c = cos(angle + time) * radius;
  float s = sin(angle + time) * radius;

  vec2 aspect = vec2(1, resolution.x / resolution.y);
  vec2 xy = vec2(c, s);
  gl_Position = vec4(xy * aspect, 0, 1);
  gl_PointSize = 2.0 + length(xy) * 20. * resolution.x / 1600.0;

  float hue = (time * 0.03 + count * 1.001);
  float cPulse = pow(rPulse, 2.0);
  float invCPulse = 1. - cPulse;
  v_color = mix(vec4(hsv2rgb(vec3(hue, invCPulse, 1.)), 1), background, radius - cPulse);
}
)");
	Renderer::getInstance()->setAudioFile("../../res/music/test.wav");
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
	float printTime = 0.0f;
	uint32_t musicStartTime = SDL_GetTicks();

	//Test Camera
	Node* camNode = new Node(Renderer::getInstance()->getSceneAt(0), nullptr);
	Camera* camera = new Camera(camNode);
	Renderer::getInstance()->setActiveCamera(camera);
	camNode->setPosition(glm::vec3(0, 0, -5));
	camNode->setRotation(
		glm::angleAxis(glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f)));
	Renderer::getInstance()->getSceneAt(0)->addNode(camNode);
	Renderer::getInstance()->setDiffuseIBLIntensity(10.0f);
	
	while (running) {
		frameStart = SDL_GetTicks();
		const uint8_t* state = SDL_GetKeyboardState(nullptr);
		if (state[SDL_SCANCODE_W]) {
			camNode->setPosition(camNode->getPosition() +
				-1.0f * cam_speed * FIXED_DELTATIME *
				camNode->getFront());
		}
		if (state[SDL_SCANCODE_A]) {
			camNode->setPosition(camNode->getPosition() +
				-1.0f * cam_speed * FIXED_DELTATIME *
				camNode->getLeft());
		}
		if (state[SDL_SCANCODE_S]) {
			camNode->setPosition(camNode->getPosition() +
				cam_speed * FIXED_DELTATIME * camNode->getFront());
		}
		if (state[SDL_SCANCODE_D]) {
			camNode->setPosition(camNode->getPosition() +
				cam_speed * FIXED_DELTATIME * camNode->getLeft());
		}
		if (state[SDL_SCANCODE_E]) {
			camNode->setPosition(camNode->getPosition() +
				cam_speed * FIXED_DELTATIME * glm::vec3(0, 1, 0));
		}
		if (state[SDL_SCANCODE_Q]) {
			camNode->setPosition(camNode->getPosition() +
				cam_speed * FIXED_DELTATIME * glm::vec3(0, -1, 0));
		}
		while (SDL_PollEvent(&event)) {
			switch (event.type) {
			case SDL_KEYDOWN:
				break;
			case SDL_WINDOWEVENT:
				if (event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED) {
					Renderer::getInstance()->resize(event.window.data1, event.window.data2);
				}
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
					camNode->setRotation(glm::angleAxis(glm::radians(cam_xRot),
						-1.0f * camNode->getLeft()) *
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
		loop();

		uint32_t curTime = SDL_GetTicks();
		frameTime = curTime - frameStart;
		//fixed deltatime
		if (FIXED_DELTATIME * 1000.0 > frameTime)
		{
			SDL_Delay(FIXED_DELTATIME * 1000.0 - frameTime);
		}
		musicTime = (curTime - musicStartTime) / 1000.0f;
		if (musicTime > printTime) {
			printTime = ceil(musicTime);
			std::cout << musicTime << std::endl;
		}
	}
}
int main(int argc, char* argv[]) {
	initialize(initialWidth, initialHeight);

	windowTestProc();

	deInitialize();
	return 0;
}
#endif

void initialize(int width, int height) {
	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		printf("SDL Initialization Fail: %s\n", SDL_GetError());
		exit(0);
	}

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
	SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4);

	// 윈도우 창 생성
	g_window =
		SDL_CreateWindow("EffectSong", SDL_WINDOWPOS_UNDEFINED,
			SDL_WINDOWPOS_UNDEFINED, width, height, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
	// OpenGL 컨텍스트 생성
	g_context = SDL_GL_CreateContext(g_window);

	Renderer::getInstance()->resize(width, height);
}

void deInitialize() {
	ArtShader::deleteInstance();
	Renderer::deleteInstance();

	SDL_GL_DeleteContext(g_context);

	SDL_DestroyWindow(g_window);
	SDL_Quit();
}

void loop() {
	Renderer::getInstance()->update(musicTime);
	Renderer::getInstance()->render();
	SDL_GL_SwapWindow(g_window);
}