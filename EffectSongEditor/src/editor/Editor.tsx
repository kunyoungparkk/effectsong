import { useEffect, useRef, useState, useCallback } from "react";

import ShaderSettings from "./ScriptEditor/ShaderSettings";
import LeftTab from "./LeftPanel/LeftTab";
import RightTab from "./RightPanel/RightTab";
import MusicImport from "./Import/MusicImport";
import GLTFImport from "./Import/GLTFImport";

import createModule from "../core/effectsong-core.js"
import * as core from '../core/effectsong-core';

import { hierarchyNodeType } from "./LeftPanel/HierarchyView.d";

import AudioPlayer from "react-h5-audio-player";
import "react-h5-audio-player/lib/styles.css";
import { floor } from "mathjs";
import {
  Grid,
  Snackbar,
  Alert,
  CircularProgress,
  Backdrop,
  Typography
} from "@mui/material";

export default function Editor() {
  const currentWidth = useRef(1000);
  const currentHeight = useRef(1000);
  const [module, setModule] = useState<core.MainModule>(null!);
  const audioRef = useRef<AudioPlayer>(null);
  const canvasRef = useRef<HTMLCanvasElement>(null);
  const canvasDivRef = useRef<HTMLDivElement>(null);

  const [notifyOpen, setNotifyOpen] = useState(false);
  const [notifySuccess, setNotifySuccess] = useState(false);
  const [notifyMessage, setNotifyMessage] = useState("");
  const [loading, setLoading] = useState(true);

  const [hierarchyData, setHierarchyData] = useState<Array<hierarchyNodeType>>([]);
  const [expandIdList, setExpandIdList] = useState<Array<string>>([]);
  const [selectedNode, setSelectedNode] = useState<core.Node | null>(null);

  const notify = useCallback((message: string, isSuccess: boolean) => {
    setNotifySuccess(isSuccess);
    setNotifyMessage(message);
    setNotifyOpen(true);
  }, [setNotifySuccess, setNotifyMessage, setNotifyOpen]);

  const getNodeById = useCallback((id: string) => {
    const idxList = id.split("-");

    let curNode: core.Node = module.Renderer.getInstance()!.getSceneAt(
      parseInt(idxList[0])
    )!;
    for (let i = 1; i < idxList.length; i++) {
      curNode = curNode.getChildAt(parseInt(idxList[i]))!;
    }
    return curNode;
  }, [module]);

  const removeSelectedNode = useCallback(() => {
    if (selectedNode === null) {
      return;
    }

    const parentNode = selectedNode.getParent();
    if (parentNode) {
      parentNode.removeChild(selectedNode);
    } else {
      module.Renderer.getInstance()!.removeScene(selectedNode as core.Scene);
    }
    setSelectedNode(null);
  }, [module, selectedNode, setSelectedNode]);

  const recursiveWriteNodes = useCallback((curNode: core.Node, id: string) => {
    //현재 노드 기록
    let currentData: hierarchyNodeType = {
      id: id,
      name: curNode.getName(),
      isSelected: false,
      children: [],
    };
    if (curNode.isAliasOf(selectedNode as core.Node)) {
      currentData.isSelected = true;
    } else {
      currentData.isSelected = false;
    }

    //0:scene, 1~: node
    for (let i = 0; i < curNode.getChildrenCount(); i++) {
      let currentChildNode = curNode.getChildAt(i);
      currentData.children.push(
        recursiveWriteNodes(currentChildNode as core.Node, id + "-" + i)
      );
    }
    return currentData;
  }, [selectedNode]);

  const updateHierarchy = useCallback(() => {
    let data = [];
    let renderer = module.Renderer.getInstance() as core.Renderer;
    for (let i = 0; i < renderer.getSceneCount(); i++) {
      data.push(recursiveWriteNodes(renderer.getSceneAt(i) as core.Node, i.toString()));
    }
    setHierarchyData(data);
  }, [module, setHierarchyData, recursiveWriteNodes]);

  const onSelectHierarchyObj = useCallback((event: React.SyntheticEvent, id: string, isSelected: boolean) => {
    if (!isSelected) {
      return;
    }
    setSelectedNode(getNodeById(id));
  }, [setSelectedNode, getNodeById]);

  const onResizeEngine = useCallback((width: number, height: number) => {
    if (canvasRef.current === null || canvasDivRef.current === null) {
      return;
    }

    currentWidth.current = width;
    currentHeight.current = height;

    const engineAspectRatio = parseFloat(width.toString()) / height;
    const divAspectRatio = parseFloat(canvasDivRef.current.offsetWidth.toString()) / canvasDivRef.current.offsetHeight;
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
  }, []);

  const startEngine = () => {
    //module.canvas.addEventListener("webglcontextlost", (e) => { alert('WebGL context lost. You will need to reload the page.'); e.preventDefault(); }, false);
    const startMusicPath = module.getRootPath() + "res/music/unity.mp3";
    module.Renderer.getInstance()!.setAudioFile(startMusicPath);

    let arrayBuffer = module.FS.readFile(startMusicPath);
    const blob = new Blob([arrayBuffer], { type: "audio/mpeg" });
    const url = URL.createObjectURL(blob);

    //TODO: Error!
    if (audioRef.current === null || audioRef.current.audio.current === null) {
      return;
    }

    audioRef.current.audio.current.src = url;

    module.FS.unlink(startMusicPath);

    //DefaultScene
    let scene = new module.Scene();
    scene.setName("DefaultScene");
    let camNode = new module.Node(scene, scene);
    camNode.setName("DefaultCamera");
    scene.addChild(camNode);

    let cam = new module.Camera(camNode);
    cam.fov = 90.0;
    camNode.setPosition(new module.vec3(0, 0, -5));
    camNode.setRotation(new module.quat(0, 0, 1, 0));

    camNode.setCamera(cam);
    module.Renderer.getInstance()!.addScene(scene);

    module.Renderer.getInstance()!.setActiveCamera(cam);
    updateHierarchy();

    setLoading(false);
  };

  useEffect(() => {
    if (module) {
      updateHierarchy();
    }
  }, [module, selectedNode, updateHierarchy]);

  useEffect(() => {
    createModule().then((instance: core.MainModule) => {
      (instance as any)['canvas']! = document.getElementById("canvas");

      instance.initialize(
        currentWidth.current,
        currentHeight.current,
        instance.addFunction(function () {
          return audioRef.current!.audio.current!.currentTime;
        }, "f"),
        instance.addFunction(function () {
          return audioRef.current!.isPlaying();
        }, "i")
      );
      onResizeEngine(currentWidth.current, currentHeight.current);
      setModule(instance);
    });

    //키보드 이벤트 등록
    const handleKeyDown = (e: KeyboardEvent) => {
      switch (e.key) {
      }
    };
    const handleKeyUp = (e: KeyboardEvent) => {
      switch (e.key) {
      }
    };
    const handleResize = (e: UIEvent) => {
      onResizeEngine(currentWidth.current, currentHeight.current);
    };

    // const handleScroll = (e) => { };
    // const handleMouseOver = (e) => { };
    // const handleMouseOut = (e) => { };
    window.addEventListener("keydown", handleKeyDown);
    window.addEventListener("keyup", handleKeyUp);
    window.addEventListener("resize", handleResize);
    // document.body.addEventListener("wheel", handleScroll);
    // document.body.addEventListener("mouseover", handleMouseOver);
    // document.body.addEventListener("mouseout", handleMouseOut);

    return () => {
      window.removeEventListener("keydown", handleKeyDown);
      window.removeEventListener("keyup", handleKeyUp);
      window.removeEventListener("resize", handleResize);
      //TODO: removeFunction
      module.deInitialize();
      //TODO: How to remove module?
      //setModule(null);
    };
  }, []);

  //모듈 초기화
  useEffect(() => {
    if (!module) {
      return;
    }
    startEngine();
  }, [module]);

  return (
    <div className="editor">
      {/*loading popup*/}
      <Backdrop open={loading}>
        <CircularProgress color="primary" />
      </Backdrop>

      {/*notify popup*/}
      <Snackbar
        anchorOrigin={{ vertical: "top", horizontal: "center" }}
        open={notifyOpen}
        onClose={() => {
          setNotifyOpen(false);
        }}
        key="notify"
      >
        <Alert
          severity={notifySuccess ? "success" : "error"}
          variant="filled"
          sx={{ width: "100%" }}
        >
          {notifyMessage}
        </Alert>
      </Snackbar>
      {/*editor*/}
      <div className="hierarchy">
        <Typography
          variant="h5"
          sx={{
            color: "white",
            fontWeight: "bold",
            fontFamily: "Roboto, sans-serif",
            textAlign: "center",
            width: 300,
            height: 40,
          }}
        >
          EffectSong.
        </Typography>
        <LeftTab
          module={module}
          hierarchyData={hierarchyData}
          selectCallback={onSelectHierarchyObj}
          expandIdList={expandIdList}
          setExpandIdList={setExpandIdList}
          targetNode={selectedNode}
        />
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
          autoPlay={true}
        />
        {/* <div style={{ width: "90px", height: "80px" }}></div> */}
      </div>
      <div className="attribute">
        <div className="import" style={{ width: "100%", height: "40px" }}>
          <Grid
            container
            columnSpacing={{ xs: 1, sm: 2, md: 3 }}
            sx={{
              paddingTop: 0.5,
              paddingLeft: 2,
              paddingRight: 2,
              paddingBottom: 0.5,
              width: "100%",
              height: "100%",
            }}
          >
            <Grid item xs={6}>
              <GLTFImport
                module={module}
                updateHierarchy={updateHierarchy}
                notify={notify}
                setLoading={setLoading}
              />
            </Grid>
            <Grid item xs={6}>
              <MusicImport
                module={module}
                audioPlayerRef={audioRef}
                notify={notify}
                setLoading={setLoading}
              />
            </Grid>
          </Grid>
        </div>
        <RightTab
          updateHierarchy={updateHierarchy}
          module={module}
          targetNode={selectedNode}
          removeSelectedNode={removeSelectedNode}
        />
      </div>
    </div>
  );
}
