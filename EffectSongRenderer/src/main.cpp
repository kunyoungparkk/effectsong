#include "common.h"
#include <SDL.h>
#include <iostream>
#include "GLTFLoader.h"
#include "Renderer.h"
#include "Node.h"
#include "Scene.h"
#include "ArtShader.h"

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif


SDL_GLContext g_context;
SDL_Window* g_window;

void initialize(int width, int height);
void deInitialize();
void render(float musicTime, bool isPlay);

#ifdef _WIN64
float musicTime = 0.0f;
int initialWidth = 1000;
int initialHeight = 800;

void windowTestProc()
{
	util::loadGLTFData("../../res/2.0/DamagedHelmet/glTF/DamagedHelmet.gltf");
	util::loadGLTFData("../../res/2.0/Lantern/glTF/Lantern.gltf");
	util::loadGLTFData("../../res/2.0/WaterBottle/glTF/WaterBottle.gltf");
	Renderer::getInstance()->getSceneAt(0)->getNodeAt(0)->setPosition(glm::vec3(0.0f, 1.0f, 0.0f));

	Renderer::getInstance()->getSceneAt(1)->getNodeAt(0)->setPosition(glm::vec3(-2.0f, 0.0f, 0.0f));
	Renderer::getInstance()->getSceneAt(1)->getNodeAt(0)->setScale(glm::vec3(0.2f, 0.2f, 0.2f));

	Renderer::getInstance()->getSceneAt(2)->getNodeAt(0)->setPosition(glm::vec3(2.0f, 1.0f, 0.0f));
	Renderer::getInstance()->getSceneAt(2)->getNodeAt(0)->setScale(glm::vec3(3.0f));
	//Renderer::getInstance()->setBackgroundColor(glm::vec4(0.5f, 0.5f, 0.0f, 1.0f));

	ArtShader::getInstance()->setVertexCount(50000);
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
    float num = mod(vertexId, 2.0);
    float point = mod(floor(vertexId / 2.0) + mod(vertexId, 2.0) * STEP, NUM_SEGMENTS);
    float count = floor(vertexId / NUM_POINTS);
    float offset = count * 0.02;
    float angle = point * PI * 2.0 / NUM_SEGMENTS - offset;
    float spread = 0.02;
    float off = 0.1;
    float speed = count * 0.004;
    float snd = 1.0;

    // ��/�� ���� �ؽ�ó���� ���� ���
    if (num < 0.9) {
		off = 0.2;
    } else {
		off = 0.1;
    }
        snd *= (
            texture(sound, vec2(off + spread * 0., speed)).r +
            texture(sound, vec2(off + spread * 1., speed)).r +
            texture(sound, vec2(off + spread * 2., speed)).r) / 3.;
	
	float leftTarget =  texture(sound, vec2(off, 0.0)).r;
	float rightTarget =  texture(sound2, vec2(off, 0.0)).r;
	
    // ���� ���� ��� ���� ���
    float rPulse = pow(snd, 5.0);
    float radius = count * 0.02 + rPulse * 0.4; // ���� ����

    // ���� �� ȸ�� �ӵ� ����ȭ
    float rotationSpeed = num < 0.9 ? 1.0 : -1.0; // ��/�� �ݴ� ����
    float c = cos(angle + time * rotationSpeed) * radius;
    float s = sin(angle + time * rotationSpeed) * radius;

    vec2 aspect = vec2(1, resolution.x / resolution.y);
    vec2 xy = vec2(c, s);

    // �¿� �и� ����
    gl_Position = vec4(xy * aspect, 0, 1);
    gl_Position.x = num < 0.9 
        ? gl_Position.x * 0.5 - 0.5 // �������� �̵�
        : gl_Position.x * 0.5 + 0.5; // ���������� �̵�
    
    gl_PointSize = 2.0 + length(xy) * 20. * resolution.x / 1600.0;

    // ���� ����ȭ (��/�� ���� �ٸ� ���� �迭)
    float hue = time * 0.03 + count * 1.001 + (num < 0.9 ? 0.0 : 0.5); // ������ ���� �̵�
    float cPulse = pow(rPulse, 2.0);
    float invCPulse = 1.0 - cPulse;
    vec4 color = vec4(hsv2rgb(vec3(hue, invCPulse, 1.0)), 1.0);
    v_color = mix(color, background, radius - cPulse);
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
					//TODO: 45���� �Ѿ�� �̻��ϰ� ��鸮�� ���� �ذ�
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
		render(musicTime, true);

		uint32_t curTime = SDL_GetTicks();
		frameTime = curTime - frameStart;
		//fixed deltatime
		if (FIXED_DELTATIME * 1000.0 > frameTime)
		{
			SDL_Delay((uint32_t)(FIXED_DELTATIME * 1000.0) - frameTime);
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

	// ������ â ����
	g_window =
		SDL_CreateWindow("EffectSong", SDL_WINDOWPOS_UNDEFINED,
			SDL_WINDOWPOS_UNDEFINED, width, height, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
	// OpenGL ���ؽ�Ʈ ����
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

void render(float musicTime, bool isPlay) {
	Renderer::getInstance()->update(musicTime, isPlay);
	Renderer::getInstance()->render();
	SDL_GL_SwapWindow(g_window);
}