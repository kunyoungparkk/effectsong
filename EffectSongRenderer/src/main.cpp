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
	util::loadGLTFData("../../res/2.0/Lantern/glTF/Lantern.gltf");
	util::loadGLTFData("../../res/2.0/WaterBottle/glTF/WaterBottle.gltf");
	Renderer::getInstance()->getSceneAt(0)->getNodeAt(0)->setPosition(glm::vec3(0.0f, 1.0f, 0.0f));

	Renderer::getInstance()->getSceneAt(1)->getNodeAt(0)->setPosition(glm::vec3(-2.0f, 0.0f, 0.0f));
	Renderer::getInstance()->getSceneAt(1)->getNodeAt(0)->setScale(glm::vec3(0.2f, 0.2f, 0.2f));

	Renderer::getInstance()->getSceneAt(2)->getNodeAt(0)->setPosition(glm::vec3(2.0f, 1.0f, 0.0f));
	Renderer::getInstance()->getSceneAt(2)->getNodeAt(0)->setScale(glm::vec3(3.0f));
	//Renderer::getInstance()->setBackgroundColor(glm::vec4(0.5f, 0.5f, 0.0f, 1.0f));

	ArtShader::getInstance()->setVertexCount(50000);
	ArtShader::getInstance()->setPrimitiveMode(GL_TRIANGLES);
	ArtShader::getInstance()->setVertexShader(R"(
// Perlin 노이즈 함수
float hash(float n) {
    return fract(sin(n) * 43758.5453123);
}

float noise(vec2 p) {
    vec2 i = floor(p);
    vec2 f = fract(p);
    f = f * f * (3.0 - 2.0 * f);
    return mix(
        mix(hash(i.x + i.y * 57.0), hash(i.x + 1.0 + i.y * 57.0), f.x),
        mix(hash(i.x + (i.y + 1.0) * 57.0), hash(i.x + 1.0 + (i.y + 1.0) * 57.0), f.x),
        f.y
    );
}

// 원형 방사 효과
float radialGradient(vec2 uv, vec2 center, float radius, float intensity) {
    float dist = length(uv - center);
    return exp(-dist * intensity) * smoothstep(radius, 0.0, dist);
}

// 소용돌이 효과
vec2 swirl(vec2 uv, vec2 center, float strength, float offset) {
    vec2 centeredUV = uv - center;
    float angle = atan(centeredUV.y, centeredUV.x) + strength * length(centeredUV) + offset;
    float radius = length(centeredUV);
    return center + vec2(cos(angle), sin(angle)) * radius;
}

// 다중 별 생성
vec3 multipleStars(vec2 uv, vec2[15] centers, float[15] radii, float[15] intensities) {
    vec3 starColor = vec3(0.0);
    for (int i = 0; i < 15; i++) {
        float starGlow = radialGradient(uv, centers[i], radii[i], intensities[i]);
        vec3 singleStarColor = mix(
            vec3(1.0, 0.9, 0.3), // 중심부 강렬한 노란빛
            vec3(1.0, 0.7, 0.2), // 외곽부 따뜻한 주황빛
            pow(starGlow, 1.0)
        );
        starColor += singleStarColor * starGlow; // 별빛 중첩
    }
    return starColor * 1.2; // 별빛 강조
}

void main() {
    // 화면 비율과 격자 크기
    float aspect = resolution.x / resolution.y;
    float gridSize = 100.0; // 격자 해상도
    float numTrianglesPerQuad = 2.0;
    float numVerticesPerQuad = numTrianglesPerQuad * 3.0;
    float quadId = floor(vertexId / numVerticesPerQuad);
    float vertexInQuad = mod(vertexId, numVerticesPerQuad);

    // 격자에서 사각형 위치 계산
    float quadX = mod(quadId, gridSize - 1.0);
    float quadY = floor(quadId / (gridSize - 1.0));

    // 사각형의 네 정점 계산
    vec2 p0 = vec2(quadX, quadY) / gridSize * 2.0 - 1.0; // 좌상단
    vec2 p1 = vec2(quadX + 1.0, quadY) / gridSize * 2.0 - 1.0; // 우상단
    vec2 p2 = vec2(quadX, quadY + 1.0) / gridSize * 2.0 - 1.0; // 좌하단
    vec2 p3 = vec2(quadX + 1.0, quadY + 1.0) / gridSize * 2.0 - 1.0; // 우하단

    vec2 pos;
    if (vertexInQuad < 3.0) {
        // 첫 번째 삼각형
        if (vertexInQuad == 0.0) pos = p0;
        if (vertexInQuad == 1.0) pos = p1;
        if (vertexInQuad == 2.0) pos = p2;
    } else {
        // 두 번째 삼각형
        if (vertexInQuad == 3.0) pos = p2;
        if (vertexInQuad == 4.0) pos = p1;
        if (vertexInQuad == 5.0) pos = p3;
    }

    // 소용돌이 확장 (NDC 전체 덮음)
    vec2 primarySwirl = swirl(pos * 1.5, vec2(0.0, 0.0), 6.0, time * 0.5);

    // 다중 별 설정
    vec2 starCenters[15] = vec2[15](
        vec2(0.0, 0.4), vec2(-0.6, 0.7), vec2(0.5, 0.5), vec2(-0.8, -0.2), vec2(0.7, -0.3), 
        vec2(-0.3, -0.5), vec2(0.6, 0.8), vec2(-0.9, 0.2), vec2(0.4, -0.6), vec2(-0.4, 0.1),
        vec2(0.2, -0.8), vec2(-0.6, 0.3), vec2(0.8, 0.9), vec2(-0.7, -0.7), vec2(0.1, 0.2)
    );
    float starRadii[15] = float[15](0.25, 0.2, 0.22, 0.18, 0.2, 0.15, 0.18, 0.17, 0.14, 0.16, 0.13, 0.19, 0.15, 0.12, 0.21);
    float starIntensities[15] = float[15](8.0, 7.0, 7.5, 6.0, 6.5, 5.0, 6.5, 7.2, 5.5, 5.8, 6.0, 7.0, 6.2, 5.5, 6.8);

    vec3 starColors = multipleStars(primarySwirl, starCenters, starRadii, starIntensities);

    // 구름과 배경 색상
    float height = noise(primarySwirl * 5.0 + time * 0.1) * 0.1;
    vec3 cloudColor = mix(
        vec3(0.1, 0.1, 0.3),  // 어두운 남색 구름
        vec3(0.3, 0.4, 0.6),  // 은은한 파란 구름
        height
    ) * 0.4; // 구름 밝기 조정

    // 최종 색상 조합
    vec3 finalColor = background.rgb * 0.3 + cloudColor + starColors * (0.5 + volume / 3.0);

    // 색상 클램핑
    finalColor = clamp(finalColor, 0.0, 1.0);

    // 빛나는 효과 추가
    finalColor += vec3(0.03 * abs(sin(time * 2.0 + vertexId * 0.1)));

    // 출력 색상
    v_color = vec4(finalColor, 1.0);

    // gl_Position 설정
    gl_Position = vec4(primarySwirl * vec2(aspect, 1.0), height, 1.0);
}
)");
	Renderer::getInstance()->setAudioFile("../../res/music/badguy.wav");
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