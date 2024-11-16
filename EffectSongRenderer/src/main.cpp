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
	ArtShader::getInstance()->setVertexCount(90000);
	ArtShader::getInstance()->setPrimitiveMode(GL_TRIANGLES);
	ArtShader::getInstance()->setVertexShader(R"(
vec3 gSunColor = vec3(1.0, 1.2, 1.4) * 10.1;  

vec3 gSkyTop =  vec3( 0.1, 0.2, 0.8 ) * 0.5;
vec3 gSkyBottom = vec3( 0.5, 0.8, 1.0 ) * 1.5;

vec3 gCubeColor = vec3(1.0, 1.0, 1.0);
float gExposure = 0.3;

float gCubeColorRandom = 0.0;

#define MOVE_OUTWARDS

float fAOAmount = 0.8;
float gFloorHeight = -1.0;
float g_cameraFar = 1000.0;

#define PI radians( 180.0 )


vec3 GetSunDir()
{
  	return normalize( vec3( 20.0, 40.3, -10.4 ) );
}

struct SceneVertex
{
  	vec3 vWorldPos;
  	vec3 vColor;
  	float fAlpha;
};


float GetCosSunRadius()
{
  return 0.01;
}


float GetSunIntensity()
{  	
  	return 0.001;
}


vec3 GetSkyColor( vec3 vViewDir )
{
	return mix( gSkyBottom, gSkyTop, max( 0.0, vViewDir.y ) );
}
const float g_cubeFaces = 6.0;
const float g_cubeVerticesPerFace = ( 2.0 * 3.0 );
const float g_cubeVertexCount =	( g_cubeVerticesPerFace * g_cubeFaces );

//                   6          7
//                    +----------+
//                   /|         /|
//                2 / |       3/ |
//                 +----------+  |
//                 |  |       |  |
//      Y   Z      | 4|       | 5|
//                 |  +-------|--+
//      ^ /        | /        | /
//      |/        0|/        1|/
//      +--> X     +----------+
  
vec3 GetCubeVertex( float fVertexIndex )
{
  	float f = fVertexIndex / 8.0;
	return vec3(
      mix(-1., 1., step(0.5, fract(f * 4.))),
      mix(-1., 1., step(0.5, fract(f * 2.))), 
      mix(-1., 1., step(0.5, fract(f))));  
}


void GetCubeVertex( const float vertexIndex, const mat4 mat, out vec3 vWorldPos, out vec3 vWorldNormal )
{
  	float fFaceIndex = floor( vertexIndex / g_cubeFaces );

  	vec3 v0, v1, v2, v3;
  
  	if ( fFaceIndex < 0.5 )
    {
      	v0 = GetCubeVertex( 0.0 );
      	v1 = GetCubeVertex( 2.0 );
      	v2 = GetCubeVertex( 3.0 );
      	v3 = GetCubeVertex( 1.0 );
    }
  	else if ( fFaceIndex < 1.5 )
    {
      	v0 = GetCubeVertex( 5.0 );
      	v1 = GetCubeVertex( 7.0 );
      	v2 = GetCubeVertex( 6.0 );
      	v3 = GetCubeVertex( 4.0 );
    }
  	else if ( fFaceIndex < 2.5 )
    {
      	v0 = GetCubeVertex( 1.0 );
      	v1 = GetCubeVertex( 3.0 );
      	v2 = GetCubeVertex( 7.0 );
      	v3 = GetCubeVertex( 5.0 );
    }
  	else if ( fFaceIndex < 3.5 )
    {
      	v0 = GetCubeVertex( 4.0 );
      	v1 = GetCubeVertex( 6.0 );
      	v2 = GetCubeVertex( 2.0 );
      	v3 = GetCubeVertex( 0.0 );
    }
  	else if ( fFaceIndex < 4.5 )
    {
      	v0 = GetCubeVertex( 2.0 );
      	v1 = GetCubeVertex( 6.0 );
      	v2 = GetCubeVertex( 7.0 );
      	v3 = GetCubeVertex( 3.0 );
    }
  	else
    {
      	v0 = GetCubeVertex( 1.0 );
      	v1 = GetCubeVertex( 5.0 );
      	v2 = GetCubeVertex( 4.0 );
      	v3 = GetCubeVertex( 0.0 );
    }
  	v0 = (mat * vec4(v0, 1)).xyz;
  	v1 = (mat * vec4(v1, 1)).xyz;
  	v2 = (mat * vec4(v2, 1)).xyz;
  	v3 = (mat * vec4(v3, 1)).xyz;
  
  	float fFaceVertexIndex = mod( vertexIndex, 6.0 );
  
  	if ( fFaceVertexIndex < 0.5 )
    {
	  	vWorldPos = v0;
    }
  	else if ( fFaceVertexIndex < 1.5 )
    {
	  	vWorldPos = v1;
    }
  	else if ( fFaceVertexIndex < 2.5 )
    {
	  	vWorldPos = v2;
    }
  	else if ( fFaceVertexIndex < 3.5 )
    {
	  	vWorldPos = v0;
    }
  	else if ( fFaceVertexIndex < 4.5 )
    {
	  	vWorldPos = v2;
    }
  	else
    {
	  	vWorldPos = v3;
    }
  
  	vWorldNormal = normalize( cross( v1 - v0, v2 - v0 ) );  
}


vec3 GetSunLighting( const vec3 vNormal )
{
  	vec3 vLight = -GetSunDir();
  
  	float NdotL = max( 0.0, dot( vNormal, -vLight ) );
 	  
  	return gSunColor * NdotL;
}


vec3 GetSunSpec( const vec3 vPos, const vec3 vNormal, const vec3 vCameraPos )
{
  	vec3 vLight = -GetSunDir();

  	vec3 vView = normalize( vCameraPos - vPos );
  
  	vec3 vH = normalize( vView - vLight );
  
  	float NdotH = max( 0.0, dot( vNormal, vH ) );
  	float NdotL = max( 0.0, dot( vNormal, -vLight ) );
 
  	float f = mix( 0.01, 1.0, pow( 1.0 - NdotL, 5.0 ) );
  
  	return gSunColor * pow( NdotH, 20.0 ) * NdotL * f * 4.0;
}


vec3 GetSkyLighting( const vec3 vNormal )
{
  	vec3 vSkyLight = normalize( vec3( -1.0, -2.0, -0.5 ) );
  
  	float fSkyBlend = vNormal.y * 0.5 + 0.5;
 
  	return mix( gSkyBottom, gSkyTop, fSkyBlend );
}


void GenerateCubeVertex( const float vertexIndex, const mat4 mat, const vec3 vCubeCol, const vec3 vCameraPos, out SceneVertex outSceneVertex )
{  
  vec3 vNormal;

  GetCubeVertex( vertexIndex, mat, outSceneVertex.vWorldPos, vNormal );

  outSceneVertex.vColor = vec3( 0.0 );

  outSceneVertex.fAlpha = 1.0;  

  float h = outSceneVertex.vWorldPos.y - gFloorHeight;
  outSceneVertex.vColor += GetSkyLighting( vNormal );
  outSceneVertex.vColor *= mix( 1.0, fAOAmount, clamp( h, 0.0, 1.0 ) );

  outSceneVertex.vColor += GetSunLighting( vNormal );

  outSceneVertex.vColor *= vCubeCol;      

  outSceneVertex.vColor += GetSunSpec( outSceneVertex.vWorldPos, vNormal, vCameraPos );
}



// hash function from https://www.shadertoy.com/view/4djSRW
float hash(float p)
{
	vec2 p2 = fract(vec2(p * 5.3983, p * 5.4427));
    p2 += dot(p2.yx, p2.xy + vec2(21.5351, 14.3137));
	return fract(p2.x * p2.y * 95.4337);
}

vec3 hsv2rgb(vec3 c) {
  c = vec3(c.x, clamp(c.yz, 0.0, 1.0));
  vec4 K = vec4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);
  vec3 p = abs(fract(c.xxx + K.xyz) * 6.0 - K.www);
  return c.z * mix(K.xxx, clamp(p - K.xxx, 0.0, 1.0), c.y);
}


mat4 rotY( float angle ) {
    float s = sin( angle );
    float c = cos( angle );
  	
    return mat4( 
      c, 0,-s, 0,
      0, 1, 0, 0,
      s, 0, c, 0,
      0, 0, 0, 1);  
}


mat4 rotZ( float angle ) {
    float s = sin( angle );
    float c = cos( angle );
  	
    return mat4( 
      c,-s, 0, 0, 
      s, c, 0, 0,
      0, 0, 1, 0,
      0, 0, 0, 1); 
}

mat4 trans(vec3 trans) {
  #if 0
  return mat4(
    1, 0, 0, trans[0],
    0, 1, 0, trans[1],
    0, 0, 1, trans[2],
    0, 0, 0, 1);
  #else
  return mat4(
    1, 0, 0, 0,
    0, 1, 0, 0,
    0, 0, 1, 0,
    trans, 1);
  #endif
}

mat4 ident() {
  return mat4(
    1, 0, 0, 0,
    0, 1, 0, 0,
    0, 0, 1, 0,
    0, 0, 0, 1);
}

mat4 uniformScale(float s) {
  return mat4(
    s, 0, 0, 0,
    0, s, 0, 0,
    0, 0, s, 0,
    0, 0, 0, 1);
}

mat4 scale(vec3 s) {
  return mat4(
    s[0], 0, 0, 0,
    0, s[1], 0, 0,
    0, 0, s[2], 0,
    0, 0, 0, 1);
}

mat4 persp(float fov, float aspect, float zNear, float zFar) {
  float f = tan(PI * 0.5 - 0.5 * fov);
  float rangeInv = 1.0 / (zNear - zFar);

  return mat4(
    f / aspect, 0, 0, 0,
    0, f, 0, 0,
    0, 0, (zNear + zFar) * rangeInv, -1,
    0, 0, zNear * zFar * rangeInv * 2., 0);
}

mat4 trInv(mat4 m) {
  mat3 i = mat3(
    m[0][0], m[1][0], m[2][0], 
    m[0][1], m[1][1], m[2][1], 
    m[0][2], m[1][2], m[2][2]);
  vec3 t = -i * m[3].xyz;
    
  return mat4(
    i[0], t[0], 
    i[1], t[1],
    i[2], t[2],
    0, 0, 0, 1);
}

mat4 transpose(mat4 m) {
  return mat4(
    m[0][0], m[1][0], m[2][0], m[3][0], 
    m[0][1], m[1][1], m[2][1], m[3][1],
    m[0][2], m[1][2], m[2][2], m[3][2],
    m[0][3], m[1][3], m[2][3], m[3][3]);
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
    -dot(xAxis, eye), -dot(yAxis, eye), -dot(zAxis, eye), 1);  
  #endif
  
}


float m1p1(float v) {
  return v * 2. - 1.;
}

float p1m1(float v) {
  return v * .5 + .5;
}

float inRange(float v, float minV, float maxV) {
  return step(minV, v) * step(v, maxV);
}

float at(float v, float target) {
  return inRange(v, target - 0.1, target + 0.1);
}

float easeInOutCubic(float pos) {
  if (pos < 0.5) {
    return 0.5 * pow(pos / 0.5, 3.);
  }
  pos -= 0.5;
  pos /= 0.5;
  pos  = 1. - pos;
  return (1. - pow(pos, 3.)) * 0.5 + 0.5;
}

float inOut(float v) {
  float t = fract(v);
  if (t < 0.5) {
    return easeInOutCubic(t / 0.5);
  }
  return easeInOutCubic(2. - t * 2.);
}

const float perBlock = 4.;

void GetCubePosition( float fCubeId, float numCubes, out mat4 mat, out vec4 vCubeCol )
{  
  	float fSeed = fCubeId;
  	float fPositionBase = fCubeId;
  	float fSize = 1.0;

  	vec3 vCubeOrigin = vec3( 0.0, 0.0, 0.0 );
  
    float across = 48.;
    float down = floor(numCubes / across);
    float uId = mod(fCubeId, across);
    float vId = floor(fCubeId / across);
    float u = uId / (across);
    float v = vId / down;
    float bxId = floor(uId / perBlock);
    float bzId = floor(vId / perBlock);
    float numRows = floor(numCubes / across);
    float numBlocks = floor(numRows / perBlock);

    float uP = m1p1(u);
    float vP = m1p1(v);

    float ll = length(vec2(uP, vP * 1.5));
    float snd = texture(sound, vec2(mix(0.001, 0.0199, uP), ll * 0.3)).r;
    float beat = texture(sound, vec2(0.007, 0)).r;
  
    vCubeOrigin.x += uP * across * 1.2 + bxId * 0. ;
    float vSpace = numRows * 1.4 + numBlocks * 0.;
    float z = vP * down * 1.4 + bzId * 0.;
    vCubeOrigin.z += z; 
    float height = 1.;
    //vCubeOrigin.y += pow(sin(time + v * 9.), 1.) * pow(cos(time + u * 8.17), 1.) * 4. * inOut(time * 0.1);
    vCubeOrigin = vec3(hash(fCubeId), hash(fCubeId * 0.371), hash(fCubeId *  0.781)) * 2. - 1.;
    vCubeOrigin *= 20.0;
  
    mat  = ident();
    mat *= trans(vCubeOrigin * pow(beat, 3.) * 2.);
    //mat *= rotZ(p1m1(snd) * 10.);
    //mat *= rotY(p1m1(snd) * 10.);
    mat *= uniformScale(mix(0., 3.0, pow(clamp(mix(-0.5, 1., snd), 0., 1.), 7.)));
  
  	vec3 vRandCol;

    float st = step(0.9, snd);
    float h = 0.65; + floor(time * 0.0) * 0.1 + easeInOutCubic(fract(time * 0.1)) * 0.1;
    vCubeCol.rgb = hsv2rgb(vec3(mix(h, h, st), 
      st,//pow(snd, 0.), 
      1));
    //vCubeCol.rgb = mix(vCubeCol.rgb, vec3(1,0,0), step(0.9, snd));
    vCubeCol.rgb = mix(vCubeCol.rgb, vec3(5,0,0), step(0.999, snd));
    vCubeCol.a = 0.;vCubeOrigin.z / vSpace;
}

float goop(float t) {
  return sin(t) * sin(t * 0.27) * sin(t * 0.13) * sin(t * 0.73);
}


void main()
{ 
  	SceneVertex sceneVertex;
  
  	float fov = 1.8;
  
//  	vec3 vCameraTarget = vec3( 300, -400.6, 500.0 );
//  	vec3 vCameraPos = vec3(-45.1, 20., -0.);
  
  	vec3 vCameraTarget = vec3( 0, 0., 0.0 );
  	vec3 vCameraPos = vec3(sin(time * 0.1) * 50., sin(time * 0.17) * 80.,  cos(time * 0.1) * 50.0);
    float ca = 0.;
  
 // get sick!
    ca = time * 0.1;
  	vec3 vCameraUp = vec3( 0, 1, 0 );
  
  	vec3 vCameraForwards = normalize(vCameraTarget - vCameraPos);
  
  	float vertexIndex = vertexId;
  
  
    float fCubeId = floor( vertexIndex / g_cubeVertexCount );
    float fCubeVertex = mod( vertexIndex, g_cubeVertexCount );
    float fNumCubes = floor( vertexCount / g_cubeVertexCount );
  
    mat4 mCube;
    vec4 vCubeCol;

    GetCubePosition( fCubeId, fNumCubes, mCube, vCubeCol );

    GenerateCubeVertex( fCubeVertex, mCube, vCubeCol.xyz, vCameraPos, sceneVertex );

    mat4 m = persp(radians(45.), resolution.x / resolution.y, 0.1, 1000.);
    m *= cameraLookAt(vCameraPos, vCameraTarget, vCameraUp);
    gl_Position = m * vec4(sceneVertex.vWorldPos, 1);
    
  	// Final output color
  	float fExposure = gExposure;// min( gExposure, time * 0.1 );
  	vec3 vFinalColor = sqrt( vec3(1.0) - exp2( sceneVertex.vColor * -fExposure ) );
  
  	v_color = mix(vec4(vFinalColor, 1), background, vCubeCol.a);
  
}
)");

	util::loadGLTFData("../../res/2.0/NewFolder/untitled.gltf");
	//util::loadGLTFData("../../res/2.0/Fox/glTF/Fox.gltf");
	//util::loadGLTFData("../../res/2.0/Duck/glTF/Duck.gltf");
	//util::loadGLTFData("../../res/2.0/WaterBottle/glTF/WaterBottle.gltf");
	//util::loadGLTFData("../../res/2.0/Lantern/glTF/Lantern.gltf");
	//util::loadGLTFData("../../res/2.0/Suzanne/glTF/Suzanne.gltf");

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

	//TODO: 임시적 변수 제거
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
