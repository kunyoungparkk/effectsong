# effectsong
effectsong.com

https://discord.gg/Em6nXUnQ

오디오 비주얼라이저 및 편집기

목표: 오디오 인터랙티브 쉐이더 아트의 대중화

# 개발 요소
#### Renderer: C++ / OpenGL로 제작, WASM으로 빌드

입력된 음악 파일의 특정 속성과 glsl 파일을 바탕으로 시각 효과를 렌더링

#### Front-End: React로 제작, WASM 로드 및 연동
Visual Node, 편집 및 기타 UI, 커뮤니티 필요

#### Back-End: Spring으로 제작.
커뮤니티와 사용자의 개인정보 및 데이터, AI에 대한 API 제공. 트래픽 처리

#### AI: Chat-GPT API를 바탕으로 제작.
음성 데이터에 대한 입력 데이터 정보와, glsl 헬퍼 함수를 바탕으로, 사용자의 텍스트 명령에 대응되는 쉐이더 이펙트 코드를 작성해주는 AI를 구현

# 원칙
#### 간단한 인터페이스
복잡한 쉐이더 코드를 작성하지 않더라도, 기본적으로 제공되는 비주얼 노드를 조합해 결과물을 만들어볼 수 있는 노드 기반 에디터를 제공

#### 자동화된 비주얼 생성 기능
사용자가 텍스트로 원하는 스타일이나 색상, 효과를 입력하면 AI가 자동으로 GLSL 코드를 생성

#### 즉각적인 피드백
사용자가 음악을 업로드하면, 실시간으로 시각 효과가 음악에 반응하도록 구현

#### 사전 제작된 템플릿
초보자들이 쉽게 시작할 수 있도록 다양한 비주얼라이제이션 템플릿을 제공

# 계획

#### ~11.23
엔진 세미 프로토 완성
- wasm 바인딩 및 기본적 조작
- Music Player, mp3/wav/flac/gltf import, Hierarchy/TRS/Light/Camera 조작, Shader Script Input
- 엔진 안정화