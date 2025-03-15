#include "common.h"
#include <SDL.h>
#include <iostream>
#include "GLTFLoader.h"
#include "Renderer.h"
#include "Node.h"
#include "Scene.h"
#include "ArtShader.h"
#include "Camera.h"
#include "Light.h"

#ifdef __EMSCRIPTEN__
#include <emscripten/emscripten.h>
#include <emscripten/bind.h>
using namespace emscripten;
// int main(){
// 	return 0;
// }
#endif

SDL_GLContext g_context;
SDL_Window *g_window;

void initialize(int width, int height);
void deInitialize();
void loop(float musicTime, bool isPlay);

#ifdef _WIN64
float musicTime = 0.0f;
int initialWidth = 1000;
int initialHeight = 800;

void windowTestProc()
{
	const float targetDeltaTime = 1.0f / TARGET_FPS;

	util::loadGLTFData(EFFECTSONG_ROOT + std::string("res/2.0/LightsPunctualLamp/glTF/LightsPunctualLamp.gltf"));
	// util::loadGLTFData(EFFECTSONG_ROOT + std::string("res/2.0/DamagedHelmet/glTF/DamagedHelmet.gltf"));
	// util::loadGLTFData(EFFECTSONG_ROOT + std::string("res/2.0/DamagedHelmet/glTF-Binary/DamagedHelmet.glb"));
	// util::loadGLTFData(EFFECTSONG_ROOT + std::string("res/2.0/DamagedHelmet/glTF-Embedded/DamagedHelmet.gltf"));
	util::loadGLTFData(EFFECTSONG_ROOT + std::string("res/2.0/Duck/glTF/Duck.gltf"));
	// util::loadGLTFData(EFFECTSONG_ROOT + std::string("res/2.0/Lantern/glTF/Lantern.gltf"));
	// util::loadGLTFData(EFFECTSONG_ROOT + std::string("res/2.0/WaterBottle/glTF/WaterBottle.gltf"));
	Renderer::getInstance()->getSceneAt(0)->getChildAt(0)->setPosition(glm::vec3(0.0f, 1.0f, 0.0f));

	Renderer::getInstance()->getSceneAt(1)->getChildAt(0)->setPosition(glm::vec3(-1.0f, 0.0f, 0.0f));
	// Renderer::getInstance()->getSceneAt(2)->getNodeAt(0)->setPosition(glm::vec3(2.0f, 1.0f, 0.0f));
	// Renderer::getInstance()->getSceneAt(2)->getNodeAt(0)->setScale(glm::vec3(3.0f));
	// Renderer::getInstance()->setBackgroundColor(glm::vec4(0.5f, 0.5f, 0.0f, 1.0f));

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
    float hue = time * 0.5 + count * 1.001 + (num < 0.9 ? 0.0 : 0.5); // ������ ���� �̵�
    float cPulse = pow(rPulse, 2.0);
    float invCPulse = 1.0 - cPulse;
    vec4 color = vec4(hsv2rgb(vec3(hue, invCPulse, 1.0)), 1.0);
    v_color = mix(color, background, radius - cPulse);
}

)");
	Renderer::getInstance()->setAudioFile(EFFECTSONG_ROOT + std::string("res/music/unity.mp3"));
	SDL_Event event;
	bool running = true;
	// camera control
	float cam_speed = 2.0f;
	bool b_mouse_pressed = false;
	int32_t prev_mouse_coord[2] = {
		0,
	};
	float cam_xRot = 0.0f;
	float cam_yRot = 180.0f;
	uint32_t frameStart;
	uint32_t frameTime;
	float printTime = 0.0f;
	uint32_t musicStartTime = SDL_GetTicks();

	// Test Camera
	Node *camNode = new Node(Renderer::getInstance()->getSceneAt(0), nullptr);
	Camera *camera = new Camera(camNode);
	Renderer::getInstance()->setActiveCamera(camera);
	camNode->setPosition(glm::vec3(0, 0, -5));
	camNode->setRotation(
		glm::angleAxis(glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f)));
	Renderer::getInstance()->getSceneAt(0)->addChild(camNode);
	Renderer::getInstance()->setDiffuseIBLIntensity(10.0f);

	while (running)
	{
		frameStart = SDL_GetTicks();
		const uint8_t *state = SDL_GetKeyboardState(nullptr);
		if (state[SDL_SCANCODE_W])
		{
			camNode->setPosition(camNode->getPosition() +
								 -1.0f * cam_speed * targetDeltaTime *
									 camNode->getFront());
		}
		if (state[SDL_SCANCODE_A])
		{
			camNode->setPosition(camNode->getPosition() +
								 -1.0f * cam_speed * targetDeltaTime *
									 camNode->getLeft());
		}
		if (state[SDL_SCANCODE_S])
		{
			camNode->setPosition(camNode->getPosition() +
								 cam_speed * targetDeltaTime * camNode->getFront());
		}
		if (state[SDL_SCANCODE_D])
		{
			camNode->setPosition(camNode->getPosition() +
								 cam_speed * targetDeltaTime * camNode->getLeft());
		}
		if (state[SDL_SCANCODE_E])
		{
			camNode->setPosition(camNode->getPosition() +
								 cam_speed * targetDeltaTime * glm::vec3(0, 1, 0));
		}
		if (state[SDL_SCANCODE_Q])
		{
			camNode->setPosition(camNode->getPosition() +
								 cam_speed * targetDeltaTime * glm::vec3(0, -1, 0));
		}
		while (SDL_PollEvent(&event))
		{
			switch (event.type)
			{
			case SDL_KEYDOWN:
				break;
			case SDL_WINDOWEVENT:
				if (event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED)
				{
					Renderer::getInstance()->resize(event.window.data1, event.window.data2);
				}
				break;
			case SDL_MOUSEBUTTONDOWN:
				b_mouse_pressed = true;
				prev_mouse_coord[0] = event.button.x;
				prev_mouse_coord[1] = event.button.y;
				break;
			case SDL_MOUSEMOTION:
				if (b_mouse_pressed)
				{
					cam_xRot += event.button.y - prev_mouse_coord[1];
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
		loop(musicTime, true);

		uint32_t curTime = SDL_GetTicks();
		frameTime = curTime - frameStart;
		// fixed deltatime
		if (1000.0 / TARGET_FPS > frameTime)
		{
			SDL_Delay((uint32_t)(1000.0 / TARGET_FPS) - frameTime);
		}
		musicTime = (curTime - musicStartTime) / 1000.0f;
		if (musicTime > printTime)
		{
			printTime = ceil(musicTime);
			std::cout << musicTime << std::endl;
		}
	}
}
int main(int argc, char *argv[])
{
	initialize(initialWidth, initialHeight);

	windowTestProc();

	deInitialize();
	return 0;
}

#elif defined(__EMSCRIPTEN__)
float (*jsGetCurrentTime)(void) = nullptr;
bool (*jsGetIsPlay)(void) = nullptr;
void loopWeb()
{
	float musicTime = jsGetCurrentTime();
	bool isPlay = jsGetIsPlay();
	loop(musicTime, isPlay);
}
void initializeWeb(int width, int height, int getCurrentTimePtr, int getIsPlayPtr)
{
	jsGetCurrentTime = (float (*)(void))getCurrentTimePtr;
	jsGetIsPlay = (bool (*)(void))getIsPlayPtr;
	initialize(width, height);
	// TARGET_FPS
	emscripten_set_main_loop(loopWeb, 0, false);
}
std::string getRootPath()
{
	return EFFECTSONG_ROOT;
}
EMSCRIPTEN_BINDINGS(CORE)
{
	emscripten::function("initialize", &initializeWeb);
	emscripten::function("deInitialize", &deInitialize);
	emscripten::function("loadGLTFData", &util::loadGLTFData, allow_raw_pointers());
	emscripten::function("getRootPath", &getRootPath, allow_raw_pointers());
}
EMSCRIPTEN_BINDINGS(GLTF)
{
	class_<Node>("Node")
		.constructor<Scene *, Node *>()
		.property("m_bAudioReactiveScale", &Node::m_bAudioReactiveScale)
		.property("m_reactiveOriginScale", &Node::m_reactiveOriginScale)
		.property("m_reactiveChangingScale", &Node::m_reactiveChangingScale)
		.function("getPosition", &Node::getPosition, allow_raw_pointers())
		.function("getRotation", &Node::getRotation, allow_raw_pointers())
		.function("getEulerRotation", &Node::getEulerRotation, allow_raw_pointers())
		.function("getScale", &Node::getScale, allow_raw_pointers())
		.function("getFront", &Node::getFront, allow_raw_pointers())
		.function("getLeft", &Node::getLeft, allow_raw_pointers())
		.function("setPosition", &Node::setPosition, allow_raw_pointers())
		.function("setRotation", &Node::setRotation, allow_raw_pointers())
		.function("setRotationByEuler", &Node::setRotationByEuler, allow_raw_pointers())
		.function("setScale", &Node::setScale, allow_raw_pointers())
		.function("getGlobalPosition", &Node::getGlobalPosition, allow_raw_pointers())
		.function("getGlobalRotation", &Node::getGlobalRotation, allow_raw_pointers())
		.function("getGlobalScale", &Node::getGlobalScale, allow_raw_pointers())
		.function("getGlobalFront", &Node::getGlobalFront, allow_raw_pointers())
		.function("getGlobalLeft", &Node::getGlobalLeft, allow_raw_pointers())
		.function("getLight", &Node::getLight, allow_raw_pointers())
		.function("setLight", &Node::setLight, allow_raw_pointers())
		.function("getCamera", &Node::getCamera, allow_raw_pointers())
		.function("setCamera", &Node::setCamera, allow_raw_pointers())
		.function("getName", &Node::getName, allow_raw_pointers())
		.function("setName", &Node::setName, allow_raw_pointers())
		.function("addChild", &Node::addChild, allow_raw_pointers())
		.function("removeChild", &Node::removeChild, allow_raw_pointers())
		.function("getParent", &Node::getParent, allow_raw_pointers())
		.function("setParent", &Node::setParent, allow_raw_pointers())
		.function("getChildAt", &Node::getChildAt, allow_raw_pointers())
		.function("getChildByName", &Node::getChildByName, allow_raw_pointers())
		.function("getChildrenCount", &Node::getChildrenCount);
	class_<Scene, base<Node>>("Scene")
		.constructor<>()
		.function("addLight", &Scene::addLight, allow_raw_pointers())
		.function("removeLight", &Scene::removeLight, allow_raw_pointers())
		.function("addCamera", &Scene::addCamera, allow_raw_pointers())
		.function("removeCamera", &Scene::removeCamera, allow_raw_pointers())
		.function("getCameraAt", &Scene::getCameraAt, allow_raw_pointers())
		.function("getCameraCount", &Scene::getCameraCount);
	enum_<ProjectionType>("ProjectionType")
		.value("PERSPECTIVE", ProjectionType::PERSPECTIVE)
		.value("ORTHOGRAPHIC", ProjectionType::ORTHOGRAPHIC);
	class_<Camera>("Camera")
		.constructor<Node *>()
		.property("projectionType", &Camera::projectionType)
		.property("aspectRatio", &Camera::aspectRatio)
		.property("fov", &Camera::fov)
		.property("xMag", &Camera::xMag)
		.property("yMag", &Camera::yMag)
		.property("zFar", &Camera::zFar)
		.property("zNear", &Camera::zNear)
		.property("name", &Camera::name)
		.function("getNode", &Camera::getNode, allow_raw_pointers());
	enum_<Light::LightType>("LightType")
		.value("NONE", Light::LightType::NONE)
		.value("DIRECTIONAL_LIGHT", Light::LightType::DIRECTIONAL_LIGHT)
		.value("POINT_LIGHT", Light::LightType::POINT_LIGHT)
		.value("SPOT_LIGHT", Light::LightType::SPOT_LIGHT);
	class_<Light>("Light")
		.constructor<Node *>()
		.property("color", &Light::color)
		.property("intensity", &Light::intensity)
		.property("range", &Light::range)
		.property("innerConeAngle", &Light::innerConeAngle)
		.property("outerConeAngle", &Light::outerConeAngle)
		.property("lightType", &Light::lightType)
		.property("name", &Light::name)
		.function("getNode", &Light::getNode, allow_raw_pointers());
	// class_<Material>("Material")
	// 	.function("bind", &Material::bind);
}
EMSCRIPTEN_BINDINGS(SINGLETON)
{
	class_<Renderer>("Renderer")
		.class_function("getInstance", &Renderer::getInstance, allow_raw_pointers())
		.function("addScene", &Renderer::addScene, allow_raw_pointers())
		.function("removeScene", &Renderer::removeScene, allow_raw_pointers())
		.function("getSceneAt", &Renderer::getSceneAt, allow_raw_pointers())
		.function("getSceneByName", &Renderer::getSceneByName, allow_raw_pointers())
		.function("getSceneCount", &Renderer::getSceneCount)
		.function("setActiveCamera", &Renderer::setActiveCamera, allow_raw_pointers())
		.function("getActiveCamera", &Renderer::getActiveCamera, allow_raw_pointers())
		// .function("getMaterial", &Renderer::getMaterial, allow_raw_pointers())
		// .function("addMaterial", &Renderer::addMaterial, allow_raw_pointers())
		// .function("getTexture", &Renderer::getTexture, allow_raw_pointers())
		.function("getWidth", &Renderer::getWidth)
		.function("getHeight", &Renderer::getHeight)
		.function("resize", &Renderer::resize)
		.function("setAudioFile", &Renderer::setAudioFile, allow_raw_pointers())
		.function("getBackgroundColor", &Renderer::getBackgroundColor, allow_raw_pointers())
		.function("setBackgroundColor", &Renderer::setBackgroundColor, allow_raw_pointers())
		.function("getDiffuseIBLIntensity", &Renderer::getDiffuseIBLIntensity)
		.function("setDiffuseIBLIntensity", &Renderer::setDiffuseIBLIntensity)
		.function("getCurrentEnergy", &Renderer::getCurrentEnergy);
	class_<ArtShader>("ArtShader")
		.class_function("getInstance", &ArtShader::getInstance, allow_raw_pointers())
		.function("getVertexShader", &ArtShader::getVertexShader, allow_raw_pointers())
		.function("getUserVertexShader", &ArtShader::getUserVertexShader, allow_raw_pointers())
		.function("setVertexShader", &ArtShader::setVertexShader, allow_raw_pointers())
		.function("getPrimitiveMode", &ArtShader::getPrimitiveMode, allow_raw_pointers())
		.function("setPrimitiveMode", &ArtShader::setPrimitiveMode, allow_raw_pointers())
		.function("getVertexCount", &ArtShader::getVertexCount, allow_raw_pointers())
		.function("setVertexCount", &ArtShader::setVertexCount, allow_raw_pointers())
		.function("getLastCompileError", &ArtShader::getLastCompileError, allow_raw_pointers());
}

EMSCRIPTEN_BINDINGS(BIND_GLM)
{
	class_<glm::vec2>("vec2")
		.constructor<float, float>()
		.property("x", &glm::vec2::x)
		.property("y", &glm::vec2::y);
	class_<glm::vec3>("vec3")
		.constructor<float, float, float>()
		.property("x", &glm::vec3::x)
		.property("y", &glm::vec3::y)
		.property("z", &glm::vec3::z);
	class_<glm::vec4>("vec4")
		.constructor<float, float, float, float>()
		.property("x", &glm::vec4::x)
		.property("y", &glm::vec4::y)
		.property("z", &glm::vec4::z)
		.property("w", &glm::vec4::w);
	class_<glm::quat>("quat")
		.constructor<float, float, float, float>()
		.property("x", &glm::quat::x)
		.property("y", &glm::quat::y)
		.property("z", &glm::quat::z)
		.property("w", &glm::quat::w);
}
#endif

void initialize(int width, int height)
{
	// #ifdef __EMSCRIPTEN__
	//	emscripten_html5_remove_all_event_listeners();
	// #endif
	SDL_SetHint(SDL_HINT_EMSCRIPTEN_KEYBOARD_ELEMENT, "#canvas");
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		printf("SDL Initialization Fail: %s\n", SDL_GetError());
		exit(0);
	}

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
	SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4);

	uint32_t flags = SDL_WINDOW_OPENGL;
#if _WIN64
	flags |= SDL_WINDOW_RESIZABLE;
#endif
	g_window =
		SDL_CreateWindow("EffectSong", SDL_WINDOWPOS_UNDEFINED,
						 SDL_WINDOWPOS_UNDEFINED, width, height, flags);
	g_context = SDL_GL_CreateContext(g_window);
	Renderer::getInstance()->resize(width, height);
}

void deInitialize()
{
	ArtShader::deleteInstance();
	Renderer::deleteInstance();

	SDL_GL_DeleteContext(g_context);

	SDL_DestroyWindow(g_window);
	SDL_Quit();
}

void loop(float musicTime, bool isPlay)
{
	Renderer::getInstance()->update(musicTime, isPlay);
	Renderer::getInstance()->render();
	SDL_GL_SwapWindow(g_window);
}
