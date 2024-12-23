import { useEffect, useRef, useState, useCallback } from "react";
import AudioPlayer from "react-h5-audio-player";
import "react-h5-audio-player/lib/styles.css";
import ShaderSettings from "./ScriptEditor/ShaderSettings";
import LeftTab from "./LeftPanel/LeftTab";
import RightTab from "./RightPanel/RightTab";
import Typography from "@mui/material/Typography";
import { floor } from "mathjs";
import MusicImport from "./Import/MusicImport";
import GLTFImport from "./Import/GLTFImport";
import {
  Grid,
  Snackbar,
  Alert,
  CircularProgress,
  Backdrop,
} from "@mui/material";

export default function Editor() {
  const currentWidth = useRef(1000);
  const currentHeight = useRef(1000);
  const [module, setModule] = useState(null);
  const audioRef = useRef(null);
  const canvasRef = useRef(null);
  const canvasDivRef = useRef(null);

  const [notifyOpen, setNotifyOpen] = useState(false);
  const [notifySuccess, setNotifySuccess] = useState(false);
  const [notifyMessage, setNotifyMessage] = useState("");
  const [loading, setLoading] = useState(true);

  //Hierarchy Data State
  const [hierarchyData, setHierarchyData] = useState([]);
  const [expandIdList, setExpandIdList] = useState([]);
  const [selectedNode, setSelectedNode] = useState(null);

  const notify = useCallback((message, isSuccess = false) => {
    setNotifySuccess(isSuccess);
    setNotifyMessage(message);
    setNotifyOpen(true);
  }, [setNotifySuccess, setNotifyMessage, setNotifyOpen]);

  const getNodeById = useCallback((id) => {
    const idxList = id.split("-");
    let curNode = module.Renderer.getInstance().getSceneAt(
      parseInt(idxList[0])
    );
    for (let i = 1; i < idxList.length; i++) {
      curNode = curNode.getChildAt(parseInt(idxList[i]));
    }
    return curNode;
  }, [module]);

  const removeSelectedNode = useCallback(() => {
    const parentNode = selectedNode.getParent();
    if (parentNode) {
      parentNode.removeChild(selectedNode);
    } else {
      module.Renderer.getInstance().removeScene(selectedNode);
    }
    setSelectedNode(null);
  }, [module, selectedNode, setSelectedNode]);

  const recursiveWriteNodes = useCallback((selectedNodePtr, curNode, id) => {
    //현재 노드 기록
    let currentData = {
      id: id,
      name: curNode.getName(),
      isSelected: false,
      children: [],
    };
    if (curNode.$$.ptr === selectedNodePtr) {
      currentData.isSelected = true;
    } else {
      currentData.isSelected = false;
    }

    //0:scene, 1~: node
    for (let i = 0; i < curNode.getChildrenCount(); i++) {
      let currentChildNode = curNode.getChildAt(i);
      currentData.children.push(
        recursiveWriteNodes(selectedNodePtr, currentChildNode, id + "-" + i)
      );
    }
    return currentData;
  }, []);

  const updateHierarchy = useCallback((selectedNodePtr) => {
    let data = [];
    let renderer = module.Renderer.getInstance();
    for (let i = 0; i < renderer.getSceneCount(); i++) {
      data.push(recursiveWriteNodes(selectedNodePtr, renderer.getSceneAt(i), i.toString()));
    }
    setHierarchyData(data);
  }, [module, setHierarchyData, recursiveWriteNodes]);

  const onSelectHierarchyObj = useCallback((event, id, isSelected) => {
    if (!isSelected) {
      return;
    }
    setSelectedNode(getNodeById(id));
  }, [setSelectedNode, getNodeById]);

  const onResizeEngine = useCallback((width, height) => {
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
  }, []);

  const startEngine = () => {
    //module.canvas.addEventListener("webglcontextlost", (e) => { alert('WebGL context lost. You will need to reload the page.'); e.preventDefault(); }, false);
    const startMusicPath = module.getRootPath() + "res/music/unity.mp3";
    module.Renderer.getInstance().setAudioFile(startMusicPath);

    let arrayBuffer = module.FS.readFile(startMusicPath);
    const blob = new Blob([arrayBuffer], { type: "audio/mpeg" });
    const url = URL.createObjectURL(blob);
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
    module.Renderer.getInstance().addScene(scene);

    module.Renderer.getInstance().setActiveCamera(cam);
    updateHierarchy(null);

    setLoading(false);
  };

  useEffect(() => {
    if (module) {
      updateHierarchy(selectedNode?.$$.ptr);
    }
  }, [module, selectedNode, updateHierarchy]);

  useEffect(() => {
    let tempInstance = null;
    window.createModule().then((instance) => {
      tempInstance = instance;
      instance.canvas = document.getElementById("canvas");
      instance.initialize(
        currentWidth.current,
        currentHeight.current,
        instance.addFunction(function () {
          return audioRef.current.audio.current.currentTime;
        }, "f"),
        instance.addFunction(function () {
          return audioRef.current.isPlaying();
        }, "i")
      );
      onResizeEngine(currentWidth.current, currentHeight.current);
      setModule(instance);
    });

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
      window.removeEventListener("keydown", handleKeyDown);
      window.removeEventListener("keyup", handleKeyUp);
      window.removeEventListener("resize", handleResize);
      //TODO: removeFunction
      tempInstance.deInitialize();
      setModule(null);
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
