import { useEffect, useRef, useState } from "react";
import useUtil from "./Util.js";
import AudioPlayer from "react-h5-audio-player";
import "react-h5-audio-player/lib/styles.css";
import ShaderSettings from "./ScriptEditor/ShaderSettings.js";
import LeftTab from "./LeftPanel/LeftTab.js";
import RightTab from "./RightPanel/RightTab.js";
import HierarchyView from "./LeftPanel/HierarchyView.js";
import Typography from '@mui/material/Typography';
import { floor } from "mathjs";
//import { Box, Button, Dialog, DialogActions, DialogContent, DialogContentText, DialogTitle, Modal, CircularProgress, Snackbar, Alert } from '@mui/material';
//import JSZip from 'jszip';
//import { rotationMatrix, multiply } from 'mathjs';
export default function Editor() {
  const currentWidth = useRef(1400);
  const currentHeight = useRef(900);
  const [module, setModule] = useState(null);
  const audioRef = useRef(null);
  const canvasRef = useRef(null);
  const canvasDivRef = useRef(null);

  const [leftTabIndex, setLeftTabIndex] = useState(0);
  const [rightTabIndex, setRightTabIndex] = useState(0);

  //Hierarchy Data State
  const [hierarchyData, setHierarchyData] = useState([]);
  const [expandIdList, setExpandIdList] = useState([]);
  const [selectedNode, setSelectedNode] = useState(null);
  useEffect(() => {
    if (module) {
      updateHierarchy();
    }
  }, [selectedNode]);

  const getNodeById = (id) => {
    const idxList = id.split("-");
    let curNode = module.Renderer.getInstance().getSceneAt(
      parseInt(idxList[0])
    );
    for (let i = 1; i < idxList.length; i++) {
      curNode = curNode.getChildAt(parseInt(idxList[i]));
    }
    return curNode;
  };
  const updateHierarchy = () => {
    let data = [];
    let renderer = module.Renderer.getInstance();
    for (let i = 0; i < renderer.getSceneCount(); i++) {
      data.push(recursiveWriteNodes(renderer.getSceneAt(i), i.toString()));
    }
    setHierarchyData(data);
  };
  const recursiveWriteNodes = (curNode, id) => {
    //현재 노드 기록
    let currentData = {
      id: id,
      name: curNode.getName(),
      isSelected: false,
      children: [],
    };
    if (curNode.$$.ptr === selectedNode?.$$.ptr) {
      currentData.isSelected = true;
    } else {
      currentData.isSelected = false;
    }

    //0:scene, 1~: node
    for (let i = 0; i < curNode.getChildrenCount(); i++) {
      let currentChildNode = curNode.getChildAt(i);
      currentData.children.push(
        recursiveWriteNodes(currentChildNode, id + "-" + i)
      );
    }
    return currentData;
  };
  const onSelectHierarchyObj = (event, id, isSelected) => {
    if (!isSelected) {
      return;
    }

    setSelectedNode(getNodeById(id));
  };

  const Util = useUtil();

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
    };
    const handleKeyUp = (e) => {
      switch (e.key) {
      }
    };
    const handleResize = (e) => {
      onResizeEngine(currentWidth.current, currentHeight.current);
    };

    const handleScroll = (e) => { };

    const handleMouseOver = (e) => { };

    const handleMouseOut = (e) => { };

    window.addEventListener("keydown", handleKeyDown);
    window.addEventListener("keyup", handleKeyUp);
    window.addEventListener("resize", handleResize);
    document.body.addEventListener("wheel", handleScroll);
    document.body.addEventListener("mouseover", handleMouseOver);
    document.body.addEventListener("mouseout", handleMouseOut);

    return () => {
      //키보드 이벤트 제거
      window.removeEventListener("keydown", handleKeyDown);
      window.removeEventListener("keyup", handleKeyUp);
      window.removeEventListener("resize", handleResize);
      console.log("deinitialize");
      //종료 시 addFunction 삭제 필요
      window.EFFECTSONG_CORE.deInitialize();
    };
  }, []);

  //모듈 초기화
  useEffect(() => {
    if (!module) {
      return;
    }
    startEngine();
  }, [module]);

  //엔진 초기화 시점에 호출
  function onEngineInitialized() {
    window.EFFECTSONG_CORE.canvas = document.getElementById("canvas");
    window.EFFECTSONG_CORE.initialize(currentWidth.current, currentHeight.current,
      window.EFFECTSONG_CORE.addFunction(function () { return audioRef.current.audio.current.currentTime; }, 'f'),
      window.EFFECTSONG_CORE.addFunction(function () { return audioRef.current.isPlaying(); }, 'i'),
    );
    onResizeEngine(currentWidth.current, currentHeight.current);
    setModule(window.EFFECTSONG_CORE);
  }

  const onResizeEngine = (width, height) => {
    currentWidth.current = width;
    currentHeight.current = height;

    const engineAspectRatio = parseFloat(width) / height;
    const divAspectRatio = parseFloat(
      canvasDivRef.current.offsetWidth / canvasDivRef.current.offsetHeight
    );
    canvasRef.current.width = width;
    canvasRef.current.height = height;
    if (engineAspectRatio > divAspectRatio) {
      canvasRef.current.style.setProperty("width", "100%");
      canvasRef.current.style.setProperty(
        "height",
        `${canvasDivRef.current.offsetWidth / engineAspectRatio}px`
      );
    } else {
      canvasRef.current.style.setProperty("height", "100%");
      canvasRef.current.style.setProperty(
        "width",
        `${floor(canvasDivRef.current.offsetHeight * engineAspectRatio)}px`
      );
    }
  };

  const startEngine = () => {
    //module.canvas.addEventListener("webglcontextlost", (e) => { alert('WebGL context lost. You will need to reload the page.'); e.preventDefault(); }, false);
    module.loadGLTFData(
      module.getRootPath() + "res/2.0/DamagedHelmet/glTF/DamagedHelmet.gltf"
    );
    updateHierarchy();
    module.Renderer.getInstance().setAudioFile(
      module.getRootPath() + "res/music/test.wav"
    );
    let camNode = new module.Node(
      module.Renderer.getInstance().getSceneAt(0),
      null
    );
    let cam = new module.Camera(camNode);
    camNode.setPosition(new module.vec3(0, 0, -5));
    camNode.setRotation(new module.quat(0, 0, 1, 0));
    module.Renderer.getInstance().setActiveCamera(cam);
    module.Renderer.getInstance().getSceneAt(0).addNode(camNode);
    module.loop(0.0, false);
  };

  return (
    <div className="editor">
      <div className="hierarchy">
        <Typography
          variant="h5" // 글자 크기
          sx={{
            color: 'white', // 기본 텍스트 색상
            fontWeight: 'bold',
            fontFamily: 'Roboto, sans-serif',
            textAlign: 'center',
            width: 300,
            height: 40
          }}
        >
          EffectSong.
        </Typography>
        <LeftTab
          onChangedIndex={setLeftTabIndex}
          module={module}
          hierarchyData={hierarchyData}
          selectCallback={onSelectHierarchyObj}
          expandIdList={expandIdList}
          setExpandIdList={setExpandIdList} />
      </div>
      <ShaderSettings module={module} onResizeEngine={onResizeEngine} />
      <div className="engineDiv" id="engineDiv" ref={canvasDivRef}>
        <canvas
          id="canvas"
          ref={canvasRef}
          style={{
            verticalAlign: "bottom",
          }}
          className="engineCanvas"
        ></canvas>
      </div>
      <div className="musicPlayer">
        <AudioPlayer
          ref={audioRef}
          volume={0.8}
          showSkipControls
          progressUpdateInterval={100}
          src="test.wav"
        />
        {/* <div style={{ width: "90px", height: "80px" }}></div> */}
      </div>
      <div className="attribute">
        <RightTab onChangedIndex={setRightTabIndex} module={module} targetNode={selectedNode} />
      </div>
    </div>
  );
}
