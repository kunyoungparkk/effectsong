import Anthropic from "@anthropic-ai/sdk";
import express, {Request, Response, NextFunction} from "express";
import cors from 'cors';

const anthropic = new Anthropic();

const app = express();
app.use(cors());

const port = 7777;
app.get("/stream", async (req, res) => {

  console.log(req);
  let predefinedRequest = `당신은 OpenGL ES 3.0에 정통한 전문적인 Shader 아티스트이자 창의적인 기술 전문가입니다.

### 시각적 목표 "{사용자 query}"를 다음과 같은 특성을 가진 최고 수준의 시각적 효과로 구현해주세요:
- 움직임: 매끄럽고 자연스러운 동적 표현과 화려한 애니메이션
- 색감: 세련되고 조화로운 색상 구성과 풍부한 그라데이션
- 반응성: 음악과 시간에 따른 역동적이고 유기적인 변화
- 공간감: 압도적인 깊이감과 다층적인 레이어드 효과
- 질감: 풍부한 표면 디테일과 유기적인 텍스처 표현
- 일관성: 프레임 간 부드러운 전환과 안정적인 움직임
- 정밀도: 섬세한 디테일과 미세한 변화 표현

### 개발 환경 - OpenGL ES 3.0 기반 Vertex Shader
주의사항:
- 벡터/행렬 연산시 타입이 정확히 일치해야 함
- 스위즐링 결과를 직접 연산에 사용할 수 없음 (임시 변수에 저장 후 사용)
- 모든 float는 소수점 필수 (예: 1.0)
- 배열은 상수 인덱스로만 접근 가능
- const 배열 미지원

헤더 정보:
- layout (location = 0) in float vertexId: 현재 Vertex의 인덱스
- uniform vec4 background: 배경 색상 (RGBA, 0.0~1.0 범위의 값)
- uniform float time: 음악 재생 시간 (초 단위)
- uniform float vertexCount: 총 Vertex 개수
- uniform sampler2D sound, uniform sampler2D sound2: 사운드 텍스처 데이터. 크기는 (2048, 2048)이고, y축은 시간, x축은 주파수별 fft 결과를 나타냄. y축 0이 가장 최근의 데이터이고, 매프레임 새로 채워지며 기존 데이터들은 한 줄씩 위로 스크롤되는 방식. 이를 잘 활용하면 음악의 과거까지 알 수 있기 때문에, 주변부로 파장이 서서히 퍼지는 효과를 만들어낼 수 있고, 그러한 효과를 만들어내길 추천한다. sound2는 스테레오일 때 오른쪽 사운드 텍스처, 모노라면 sound와 sound2의 값이 같다.
- uniform bool isStereo: 스테레오 여부
- out vec4 v_color: fragment shader로 전달하는 색상, fragment shader에서는 받은 색상을 바로 FragColor로 지정.
- gl_Position: NDC 위치를 결정한다.
- gl_PointSize: Points Primitive Mode일 경우에는 지정해야 함.

### Primitive mode
- 점 (Points):
  - 독립적인 파티클 효과나 별자리 등 분리된 요소들의 표현에 적합
  - gl_PointSize를 활용한 동적인 크기 변화
- 선 (Lines):
  - 연속된 선으로 패턴이나 궤적을 표현할 때 적합
  - 각 정점을 순차적으로 연결
- 삼각형 (Triangles):
  - 3D 메시 기반의 통합된 씬을 구성.
  - 3D 메시 생성 기법:
    * 구면 투영법을 통한 완전한 구체 생성
    * 극좌표계를 활용한 회전 대칭 물체
    * 사인/코사인 함수를 활용한 유기적 형상
    * 프랙탈 반복을 통한 복잡한 구조체

### 공간 활용 가이드라인
- NDC 좌표계 활용:
  * 화면 전체를 균일하게 활용
  * -1.0에서 1.0 범위를 최대한 채우면서도, 좌표가 NDC 범위를 벗어나지 않도록 주의
  * aspect ratio 고려한 왜곡 없는 투영
- 3D 공간 표현:
  * 깊이값(z축)을 효과적으로 활용
  * 원근감 있는 카메라 워크 구현
  * 물체간 깊이 관계 고려
- 화면 채움:
  * 빈 공간을 최소화하는 레이아웃
  * 시각적 요소들의 균형잡힌 배치
  * 동적 요소들의 움직임 범위 제어
- 3D 공간 구성:
  * 중첩된 레이어의 효과적 배치
  * 원근 투영을 통한 깊이감 강화
  * 카메라 이동 범위 최적화
  * 시야각과 화면비 고려한 투영

### 3D 메시 생성 가이드라인
- 기본 도형 구현:
  * 구체(Sphere): UV 매핑 기반 구면 좌표계 활용
  * 평면(Plane): 격자 기반 웨이브와 왜곡 표현
  * 원통(Cylinder): 회전체 기반 기하학적 패턴
- 복합 도형 구현:
  * 여러 기본 도형의 블렌딩
  * 거리 함수를 통한 형상 변형
  * 프랙탈 알고리즘 응용
- 정점 인덱싱 최적화:
  * 스트립 구조를 활용한 정점 재사용
  * 인접 삼각형 간 정점 공유
  * 정점 순서의 캐시 친화적 배치

### 코드 구조 가이드라인
1. 기본 유틸리티 함수
- 2D 회전 행렬 함수
- 랜덤/노이즈 함수
- 부드러운 보간 함수
2. 위치 계산
- 3D 공간의 효과적 활용:
  * 수학적 함수를 통한 3D 형상 생성
  * 복잡한 기하학적 패턴 구현
  * 파라메트릭 서피스 제작
- 메시 연결성 보장:
  * 정점 공유를 통한 연속 표면 생성
  * 빈틈없는 폴리곤 구성
  * 표면 법선 계산과 활용
- vertexId를 활용한 기본 배치
- 시간 기반 애니메이션
- 다중 레이어 움직임
- -1.0에서 1.0 사이의 정규화된 좌표계 사용

3. 색상 및 시각효과
- 그라데이션과 색상 믹싱
- 거리 기반 밝기 변화
- 시간 기반 변화 효과

4. 음악 반응성
- 볼륨에 따른 동적 효과
- 스테레오/모노 사운드 처리
- 부드러운 오디오 반응

5. GLSL ES 3.0 문법 준수
- 정확한 float 리터럴 사용 (예: 1.0, 0.5)
- 허용된 내장 함수만 사용
- 올바른 타입 캐스팅과 연산
6. primitive mode별 최적화된 접근
- Points: 독립적 파티클 처리
- Lines: 연속된 선 패턴 구성
- Triangles: 연속 메시 구조체 구현

### 최적화 요구사항
- 효율적인 수학 함수 사용
- 안정적인 랜덤 함수 구현
- 부드러운 보간과 변화
- 성능을 고려한 계산 순서
- 3D 메시 생성시 정점 재활용 최적화
- 연속된 표면 생성을 위한 인덱스 계산 최적화
- 깊이 정렬 및 컬링 고려

### 구현 스타일
- 각 요소별 명확한 주석
- 모듈화된 코드 구조
- 재사용 가능한 유틸리티 함수
- 직관적인 변수명

사용자의 요구사항인 $(request)을 바탕으로 , 위 구조를 따르되 창의적으로 해석하여 시각적으로 매력적이고 최적화된 $(primitiveMode) 프리미티브 모드의 Vertex Shader 코드를 작성해주세요. 
특히 움직임의 자연스러움, 색상의 조화, 그리고 음악과의 상호작용을 중점적으로 고려해주세요.

위에 언급한 헤더 정보는 이미 작성되어있다고 가정하고, 이를 답변에선 제외하고 그 밑부터 작성해주세요.
답변에는 코드만 포함되어야 하며, 별도의 설명은 작성하지 마세요.

        `;
  let tempReq = {primitiveMode: "POINTS", request: "별들과 항성과 함께하는 카메라무빙이 되는 역동적인 우주를 만들어줘."};

  let receivedRequest = `$(primitiveMode) = ${tempReq.primitiveMode}, 
  $(request) = "${tempReq.request}"`;


  res.setHeader('Content-Type', 'text/event-stream');
  res.setHeader('Cache-Control', 'no-cache');
  res.setHeader('Connection', 'keep-alive');

  await anthropic.messages
    .stream({
      model: "claude-3-7-sonnet-20250219",
      max_tokens: 8192,
      temperature: 0,
      messages: [
        {
          role: "user",
          content: [
            {
              type: "text",
              text: predefinedRequest + receivedRequest,
            },
          ],
        },
      ],
    })
    .on("text", (text) => {
      console.log(text);

      res.write(`data: ${text.replace(/\n/g, "\\n")}\n\n`);
    })
    .on("end", ()=>{
      res.write(`data: EFFECTSONG_AIGENERATION_COMPLETED\n\n`);
      //res.end();
    })
    .on("error", (error)=>{
      res.end();
    })
});

const server = app.listen(port, ()=>{
    console.log('server on ', port);
})
