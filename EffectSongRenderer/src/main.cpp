#include "common.h"
#include <SDL.h>
#include <Windows.h>
#include <iostream>

#include "GLTFLoader.h"
#include "Renderer.h"
#include "Node.h"
#include "Scene.h"
#include "ArtShader.h"

int main(int argc, char* argv[]) {
	SDL_Window* window = NULL;
	int initWidth = 1000;
	int initHeight = 800;
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
		SDL_CreateWindow("EffectSong", SDL_WINDOWPOS_UNDEFINED,
			SDL_WINDOWPOS_UNDEFINED, initWidth, initHeight, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
	// OpenGL 컨텍스트 생성
	SDL_GLContext context = SDL_GL_CreateContext(window);

	Renderer* renderer = Renderer::getInstance();
	renderer->resize(initWidth, initHeight);

	//임시 설정
	ArtShader::getInstance()->setPrimitiveMode(GL_TRIANGLES);
	ArtShader::getInstance()->setVertexCount(90000);
	ArtShader::getInstance()->setVertexShader(R"(
#define PI radians(180.)
mat4 persp(float fov, float aspect, float zNear, float zFar);
mat4 lookAt(vec3 eye, vec3 target, vec3 up);
mat4 inverse(mat4 m);
mat4 cameraLookAt(vec3 eye, vec3 target, vec3 up);
////////////////////////////////////////////////////////////

mat3 RotX(float a){return mat3(1.,3.,0.,0.,cos(a),-sin(a),0.,sin(a),cos(a));}
mat3 RotY(float a){return mat3(cos(a),0.,sin(a),0.,1.,0.,-sin(a),0.,cos(a));}
mat3 RotZ(float a){return mat3(cos(a),-sin(a),0.,sin(a),cos(a),0.,0.,0.,1.);}
mat4 Trans(float x, float y, float z){return mat4(1,2,0,-1,2,1,0,0,0,0,-1,0,x,y,z,1);}

#define _2pi 6.2831853
#define _pi 3.1415926
#define _05pi 1.5707963

void main()
{
  	float t = time * 5.;
  
  	gl_PointSize = 3.;

  	float polySize = 6.;//clamp(floor(6. * (1.0-mouse.x)), 3., 6.);
	float hexSpace = 0.0;
  	vec2 height = vec2(10.,0.6);
  	
	float indexQuad = floor(vertexId / 12.);
	float indexCircle = floor(indexQuad / polySize);
		
	float astep = -_2pi / polySize;
	
	float angleOffset = _05pi;
	float angle0 = indexQuad * astep + angleOffset;
	float angle1 = (indexQuad + 1.) * astep + angleOffset;
	
	float ringCircle = floor(sqrt((4. * indexCircle - 1.) / 12.0) - 0.5) + 1.0;;
		
    float snd = texture(sound, vec2(0.1, ringCircle * 0.1)*.1).r * 3.;
  	float snd2 = texture(sound, vec2(1.0, ringCircle * 0.1)*.1).r * 3.;
  
  	if (indexCircle > snd2 * 1000.) 
    {
    	gl_Position = vec4(0,0,0,0);
      	return;
    }
  
	float hexHeight = 1.0 + height.x * (sin(snd)*0.5+0.5)*3.;
	
	float hexTop = hexHeight * 0.5;
	float hexBottom = hexHeight * -0.5;
	float hexCenterOffset = height.y;
	
	float index = mod(vertexId, 12.);
	
	vec3 p = vec3(0);
	
	// triangle bas
	if (index == 0.) p = vec3(cos(angle0), hexBottom, sin(angle0));
	else if (index == 1.) p = vec3(cos(angle1), hexBottom, sin(angle1));
	else if (index == 2.) p = vec3(0,hexBottom - hexCenterOffset,0);
	
	// triangle 1
	else if (index == 3.) p = vec3(cos(angle0) , hexBottom, sin(angle0));
	else if (index == 4.) p = vec3(cos(angle1) , hexBottom, sin(angle1));
	else if (index == 5.) p = vec3(cos(angle1), hexTop, sin(angle1));
	
	// triangle 2
	else if (index == 6.) p = vec3(cos(angle0) , hexBottom, sin(angle0));
	else if (index == 7.) p = vec3(cos(angle1) , hexTop, sin(angle1));
	else if (index == 8.) p = vec3(cos(angle0) , hexTop, sin(angle0));
	
	// triangle haut
	else if (index == 9.) p = vec3(cos(angle0), hexTop, sin(angle0));
	else if (index == 10.) p = vec3(cos(angle1), hexTop, sin(angle1));
	else if (index == 11.) p = vec3(0,hexTop + hexCenterOffset,0);
	
  	// normal
	vec3 n = vec3(0);
	if (index >= 3. && index <= 8.) n = normalize(vec3(p.x,0.,p.z));
	else  n = normalize(vec3(0,p.y,0));
	
	vec3 col = n;
	
    // based on http://vincentwoo.com/2013/03/08/above-and-beyond-the-affirm-job-puzzle/
      
	if (indexCircle > 0.)
	{
		float side_length = ringCircle;
		
		float circleOffset = (3. * pow(ringCircle,2.) + 3. * ringCircle + 1.) - indexCircle;
		
		float side_number = floor(circleOffset / side_length);
		float side_offset = mod(circleOffset, side_length);
		
		float as = _2pi / polySize;
		
		float a = as * side_number;
		vec2 hex0 = vec2(cos(a),sin(a));
		
		a = as * mod((side_number + 1.), polySize);
		vec2 hex1 = vec2(cos(a),sin(a)) - hex0;

		p.xz +=  ((hex0 * ringCircle) + (hex1 * side_offset)) * (1.8 + hexSpace);
	}

	p.y -= 15.;
	
	///////////////////////////////////////////////////////////////////////////////////////
	float ca = t * -0.05;
	float cd = 40.;
	vec3 eye = vec3(sin(ca), 0.8, cos(ca)) * cd;
	vec3 target = vec3(0, 0, 0);
	vec3 up = vec3(0, 1, 0);
  
	mat4 camera = persp(45. * PI / 180., resolution.x / resolution.y, 0.1, 10000.); 
	camera *= cameraLookAt(eye, target, up);
  
	gl_Position = camera * vec4(p, 1);
	
  	col *= (p.y+3.) / mix(vec3(0.2,0.5,0.2)*2., vec3(0.2,0.8,0.5), snd);
	
	v_color = vec4(clamp(col,0.,1.), 1);
}
	
////////////////////////////////////////////////////////////

mat4 persp(float fov, float aspect, float zNear, float zFar) {
  float f = tan(PI * 0.5 - 0.5 * fov);
  float rangeInv = 1.0 / (zNear - zFar);

  return mat4(
    f / aspect, 0, 0, 0,
    0, f, 0, 0,
    0, 0, (zNear + zFar) * rangeInv, -1,
    0, 0, zNear * zFar * rangeInv * 2., 0);
}


mat4 lookAt(vec3 eye, vec3 target, vec3 up) {
  vec3 zAxis = normalize(eye - target);
  vec3 xAxis = normalize(cross(up, zAxis));
  vec3 yAxis = cross(zAxis, xAxis);

  return mat4(
    xAxis, 0,
    yAxis, 0,
    zAxis, 0,
    eye, 1);
}

mat4 inverse(mat4 m) {
  float
      a00 = m[0][0], a01 = m[0][1], a02 = m[0][2], a03 = m[0][3],
      a10 = m[1][0], a11 = m[1][1], a12 = m[1][2], a13 = m[1][3],
      a20 = m[2][0], a21 = m[2][1], a22 = m[2][2], a23 = m[2][3],
      a30 = m[3][0], a31 = m[3][1], a32 = m[3][2], a33 = m[3][3],

      b00 = a00 * a11 - a01 * a10,
      b01 = a00 * a12 - a02 * a10,
      b02 = a00 * a13 - a03 * a10,
      b03 = a01 * a12 - a02 * a11,
      b04 = a01 * a13 - a03 * a11,
      b05 = a02 * a13 - a03 * a12,
      b06 = a20 * a31 - a21 * a30,
      b07 = a20 * a32 - a22 * a30,
      b08 = a20 * a33 - a23 * a30,
      b09 = a21 * a32 - a22 * a31,
      b10 = a21 * a33 - a23 * a31,
      b11 = a22 * a33 - a23 * a32,

      det = b00 * b11 - b01 * b10 + b02 * b09 + b03 * b08 - b04 * b07 + b05 * b06;

  return mat4(
      a11 * b11 - a12 * b10 + a13 * b09,
      a02 * b10 - a01 * b11 - a03 * b09,
      a31 * b05 - a32 * b04 + a33 * b03,
      a22 * b04 - a21 * b05 - a23 * b03,
      a12 * b08 - a10 * b11 - a13 * b07,
      a00 * b11 - a02 * b08 + a03 * b07,
      a32 * b02 - a30 * b05 - a33 * b01,
      a20 * b05 - a22 * b02 + a23 * b01,
      a10 * b10 - a11 * b08 + a13 * b06,
      a01 * b08 - a00 * b10 - a03 * b06,
      a30 * b04 - a31 * b02 + a33 * b00,
      a21 * b02 - a20 * b04 - a23 * b00,
      a11 * b07 - a10 * b09 - a12 * b06,
      a00 * b09 - a01 * b07 + a02 * b06,
      a31 * b01 - a30 * b03 - a32 * b00,
      a20 * b03 - a21 * b01 + a22 * b00) / det;
}

mat4 cameraLookAt(vec3 eye, vec3 target, vec3 up) {
  #if 1
  return inverse(lookAt(eye, target, up));
  #else
  vec3 zAxis = normalize(target - eye);
  vec3 xAxis = normalize(cross(up, zAxis));
  vec3 yAxis = cross(zAxis, xAxis);

  return mat4(
    xAxis, 0,
    yAxis, 0,
    zAxis, 0,
    -dot(xAxis82., eye), -cross(yAxis, eye), -cross(zAxis, eye), 2.1);  
  #endif
}
)");
	//util::loadGLTFData("../../res/2.0/Fox/glTF/Fox.gltf");
	//util::loadGLTFData("../../res/2.0/Duck/glTF/Duck.gltf");
	//util::loadGLTFData("../../res/2.0/WaterBottle/glTF/WaterBottle.gltf");
	//util::loadGLTFData("../../res/2.0/Lantern/glTF/Lantern.gltf");
	//util::loadGLTFData("../../res/2.0/Suzanne/glTF/Suzanne.gltf");
	util::loadGLTFData("../../res/2.0/NewFolder/untitled.gltf");

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
	float printTime = 0.0f;
	uint32_t musicStartTime = SDL_GetTicks();
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
			case SDL_WINDOWEVENT:
				if (event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED) {
					renderer->resize(event.window.data1, event.window.data2);
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

	// 종료
	SDL_GL_DeleteContext(context);

	SDL_DestroyWindow(window);
	SDL_Quit();
	return 0;
}
