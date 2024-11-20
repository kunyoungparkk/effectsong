import { useEffect, useRef, useState } from 'react';
import useUtil from './Util.js'
import MusicPlayer from './MusicPlayer/MusicPlayer.js';
import ScriptEditor from './ScriptEditor/ScriptEditor.js';
import { floor } from 'mathjs';
import { Box, Button, Dialog, DialogActions, DialogContent, DialogContentText, DialogTitle, Modal, CircularProgress, Snackbar, Alert } from '@mui/material';
import JSZip from 'jszip';
import { rotationMatrix, multiply } from 'mathjs';
export default function Editor() {
    const canvasRef = useRef(null);
    const canvasDivRef = useRef(null);
    const widthVal = useRef(1200);
    const heightVal = useRef(900);

    //편집용 카메라
    const camFront = useRef([0, 0, 0]);
    const camLeft = useRef([0, 0, 0]);
    const camSpeed = 0.1;

    /*emscripten - 엔진초기화 관련*/
    const [module, setModule] = useState(null);

    const Util = useUtil();

    /*Music*/
    const [musicTime, setMusicTime] = useState(0.0);
    const [vertexShader, setVertexShader] = useState(`//shader art sample
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
	
    float rPulse = pow(snd, 5.0);
    float radius = count * 0.02 + rPulse * 0.4; 

    float rotationSpeed = num < 0.9 ? 1.0 : -1.0; 
    float c = cos(angle + time * rotationSpeed) * radius;
    float s = sin(angle + time * rotationSpeed) * radius;

    vec2 aspect = vec2(1, resolution.x / resolution.y);
    vec2 xy = vec2(c, s);

    gl_Position = vec4(xy * aspect, 0, 1);
    gl_Position.x = num < 0.9 
        ? gl_Position.x * 0.5 - 0.5 
        : gl_Position.x * 0.5 + 0.5; 
    
    gl_PointSize = 2.0 + length(xy) * 20. * resolution.x / 1600.0;

    float hue = time * 0.03 + count * 1.001 + (num < 0.9 ? 0.0 : 0.5);
    float cPulse = pow(rPulse, 2.0);
    float invCPulse = 1.0 - cPulse;
    vec4 color = vec4(hsv2rgb(vec3(hue, invCPulse, 1.0)), 1.0);
    v_color = mix(color, background, radius - cPulse);
}
`);

    useEffect(() => {
        //setShowLoading(true);
        if (!window.EFFECTSONG_CORE) {
            //모듈 초기 로드시 - 초기화
            window.initializeModule(onEngineInitialized);
        } else {
            //두번째 로드시부턴 초기화하지말고 개별적으로 startEngine 함수 호출
            onEngineInitialized();
        }
        //키보드 이벤트 등록
        const handleKeyDown = (e) => {
            switch (e.key) {
            }
        }
        const handleKeyUp = (e) => {
            switch (e.key) {
            }
        }
        const handleResize = (e) => {
            onResizeEngine();
        }

        const handleScroll = (e) => {
        }

        const handleMouseOver = (e) => {
        }

        const handleMouseOut = (e) => {
        }

        window.addEventListener('keydown', handleKeyDown);
        window.addEventListener('keyup', handleKeyUp);
        window.addEventListener('resize', handleResize);
        document.body.addEventListener('wheel', handleScroll);
        document.body.addEventListener('mouseover', handleMouseOver);
        document.body.addEventListener('mouseout', handleMouseOut);

        return () => {
            //키보드 이벤트 제거
            window.removeEventListener('keydown', handleKeyDown);
            window.removeEventListener('keyup', handleKeyUp);
            window.removeEventListener('resize', handleResize);
            console.log('웹어셈블리 모듈 해제');
            //종료 시 보유한 엔진 스마트포인터 및 addFunction 삭제 필요
            window.EFFECTSONG_CORE.deInitialize();
        };
    }, []);

    //모듈 초기화
    useEffect(() => {
        if (!module) {
            return;
        }
        startEngine();
    }, [module])

    useEffect(()=>{
        if (!module) {
            return;
        }
        module.loop(musicTime, true);
    },[musicTime])

    useEffect(()=>{
        if(!module){
            return;
        }
        let success = module.ArtShader.getInstance().setVertexShader(vertexShader);
        console.log(success);
    },[vertexShader])
   
    //엔진 초기화 시점에 호출
    function onEngineInitialized() {
        window.EFFECTSONG_CORE.canvas = document.getElementById('canvas');
        setModule(window.EFFECTSONG_CORE);
    }

    //resize된 크기에 맞게 캔버스 관련 변경 : 엔진 resize 혹은 브라우저 resize 시에 실행 필요
    const onResizeEngine = () => {
        const engineAspectRatio = parseFloat(widthVal.current) / heightVal.current;
        const divAspectRatio = parseFloat(canvasDivRef.current.offsetWidth / canvasDivRef.current.offsetHeight);
        canvasRef.current.width = widthVal.current;
        canvasRef.current.height = heightVal.current;
        if (engineAspectRatio > divAspectRatio) {
            canvasRef.current.style.setProperty('width', '100%');
            canvasRef.current.style.setProperty('height', `${canvasDivRef.current.offsetWidth / engineAspectRatio}px`);
        } else {
            canvasRef.current.style.setProperty('height', '100%');
            canvasRef.current.style.setProperty('width', `${floor(canvasDivRef.current.offsetHeight * engineAspectRatio)}px`);
        }
    }
    let time = 0;
    const startEngine = () => {
        //module.canvas.addEventListener("webglcontextlost", (e) => { alert('WebGL context lost. You will need to reload the page.'); e.preventDefault(); }, false);
        module.initialize(widthVal.current, heightVal.current);
        onResizeEngine();
        module.loadGLTFData(module.getRootPath() + "res/2.0/DamagedHelmet/glTF/DamagedHelmet.gltf");
        module.Renderer.getInstance().setAudioFile(module.getRootPath() + "res/music/test.wav");
        let camNode = new module.Node(module.Renderer.getInstance().getSceneAt(0), null);
        let cam = new module.Camera(camNode);
        camNode.setPosition(new module.vec3(0, 0, -5));
        camNode.setRotation(new module.quat(0, 0, 1, 0));
        module.Renderer.getInstance().setActiveCamera(cam);
        module.Renderer.getInstance().getSceneAt(0).addNode(camNode);
        module.ArtShader.getInstance().setVertexShader(vertexShader);
    }

    return (
        <div className="editor">
            <div className="hierarchy">
            </div>
            <div className="musicPlayer">
                <MusicPlayer setMusicTime={setMusicTime}/>
            </div>
            <div className="engineDiv" ref={canvasDivRef}>
                <canvas id="canvas" ref={canvasRef} style={{
                    verticalAlign: "bottom"
                }} className="engineCanvas"></canvas>
            </div>
            <div className="attribute"></div>
            <ScriptEditor vertexShader={vertexShader} setVertexShader={setVertexShader}></ScriptEditor>
        </div>
    );
}
