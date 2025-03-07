import { useEffect, useRef, useState, useCallback } from 'react';

import ShaderSettings from './ScriptEditor/ShaderSettings';
import LeftTab from './LeftPanel/LeftTab';
import RightTab from './RightPanel/RightTab';
import MusicImport from './Import/MusicImport';
import GLTFImport from './Import/GLTFImport';

import CoreManager from './CoreManager';
import * as core from '../core/effectsong-core';

import { hierarchyNodeType } from './common';
import { selectedNodeAtom, hierarchyDataAtom } from './atom';
import { useAtom } from 'jotai';

import AudioPlayer from 'react-h5-audio-player';
import 'react-h5-audio-player/lib/styles.css';
import { Grid, Snackbar, Alert, CircularProgress, Backdrop, Typography } from '@mui/material';

export default function Editor() {
  const currentWidth = useRef(1000);
  const currentHeight = useRef(1000);
  const audioRef = useRef<AudioPlayer>(null);
  const canvasRef = useRef<HTMLCanvasElement>(null);
  const canvasDivRef = useRef<HTMLDivElement>(null);

  const [notifyOpen, setNotifyOpen] = useState(false);
  const [notifySuccess, setNotifySuccess] = useState(false);
  const [notifyMessage, setNotifyMessage] = useState('');

  const [moduleLoaded, setModuleLoaded] = useState<boolean>(false);
  const [loading, setLoading] = useState(false);

  const [hierarchyData, setHierarchyData] = useAtom(hierarchyDataAtom);
  const [selectedNode, setSelectedNode] = useAtom(selectedNodeAtom);

  const [expandIdList, setExpandIdList] = useState<Array<string>>([]);

  const notify = useCallback(
    (message: string, isSuccess: boolean) => {
      setNotifySuccess(isSuccess);
      setNotifyMessage(message);
      setNotifyOpen(true);
    },
    [setNotifySuccess, setNotifyMessage, setNotifyOpen],
  );

  const recursiveWriteNodes = useCallback(
    (curNode: core.Node, id: string) => {
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
        currentData.children.push(recursiveWriteNodes(currentChildNode as core.Node, id + '-' + i));
      }
      return currentData;
    },
    [selectedNode],
  );

  const updateHierarchy = useCallback(() => {
    let data = [];
    let renderer = CoreManager.getInstance().getRenderer();
    for (let i = 0; i < renderer.getSceneCount(); i++) {
      data.push(recursiveWriteNodes(renderer.getSceneAt(i) as core.Node, i.toString()));
    }
    setHierarchyData(data);
  }, [setHierarchyData, recursiveWriteNodes]);


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
      canvasRef.current.style.setProperty('width', '100%');
      canvasRef.current.style.setProperty('height', `${canvasDivRef.current.offsetWidth / engineAspectRatio}px`);
    } else {
      canvasRef.current.style.setProperty('height', '100%');
      canvasRef.current.style.setProperty(
        'width',
        `${Math.floor(canvasDivRef.current.offsetHeight * engineAspectRatio)}px`,
      );
    }
  }, []);

  useEffect(() => {
    if (moduleLoaded) {
      updateHierarchy();
    }
  }, [moduleLoaded, selectedNode, updateHierarchy]);

  useEffect(() => {
    setLoading(true);
    CoreManager.getInstance()
      .initialize('canvas', currentWidth.current, currentHeight.current, audioRef)
      .then(() => {
        updateHierarchy();
        setModuleLoaded(true);
        setLoading(false);
      });
    onResizeEngine(currentWidth.current, currentHeight.current);

    //keyboard event
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
    window.addEventListener('keydown', handleKeyDown);
    window.addEventListener('keyup', handleKeyUp);
    window.addEventListener('resize', handleResize);
    // document.body.addEventListener("wheel", handleScroll);
    // document.body.addEventListener("mouseover", handleMouseOver);
    // document.body.addEventListener("mouseout", handleMouseOut);

    return () => {
      window.removeEventListener('keydown', handleKeyDown);
      window.removeEventListener('keyup', handleKeyUp);
      window.removeEventListener('resize', handleResize);
      CoreManager.getInstance().deInitialize();
    };
  }, []);

  return (
    <div className="editor">
      {/*loading popup*/}
      <Backdrop open={loading}>
        <CircularProgress color="primary" />
      </Backdrop>
      {/*notify popup*/}
      <Snackbar
        anchorOrigin={{ vertical: 'top', horizontal: 'center' }}
        open={notifyOpen}
        onClose={() => {
          setNotifyOpen(false);
        }}
        key="notify">
        <Alert severity={notifySuccess ? 'success' : 'error'} variant="filled" sx={{ width: '100%' }}>
          {notifyMessage}
        </Alert>
      </Snackbar>

      {/*editor*/}
      <div className="leftPanel">
        <Typography
          variant="h5"
          sx={{
            color: 'white',
            fontWeight: 'bold',
            fontFamily: 'Roboto, sans-serif',
            textAlign: 'center',
            width: 300,
            height: 40,
          }}>
          EffectSong.
        </Typography>
        {moduleLoaded && (
          <LeftTab
            hierarchyData={hierarchyData}
            expandIdList={expandIdList}
            setExpandIdList={setExpandIdList}
          />
        )}
      </div>
      {moduleLoaded && <ShaderSettings onResizeEngine={onResizeEngine} />}
      <div className="engineDiv" id="engineDiv" ref={canvasDivRef}>
        <canvas
          id="canvas"
          ref={canvasRef}
          style={{
            verticalAlign: 'bottom',
          }}
          className="engineCanvas"></canvas>
      </div>
      <div className="musicPlayer">
        <AudioPlayer ref={audioRef} volume={0.8} showSkipControls progressUpdateInterval={100} autoPlay={true} />
        {/* <div style={{ width: "90px", height: "80px" }}></div> */}
      </div>
      {moduleLoaded && (
        <div className="rightPanel">
          <div className="import" style={{ width: '100%', height: '40px' }}>
            <Grid
              container
              columnSpacing={{ xs: 1, sm: 2, md: 3 }}
              sx={{
                paddingTop: 0.5,
                paddingLeft: 2,
                paddingRight: 2,
                paddingBottom: 0.5,
                width: '100%',
                height: '100%',
              }}>
              <Grid item xs={6}>
                <GLTFImport updateHierarchy={updateHierarchy} notify={notify} setLoading={setLoading} />
              </Grid>
              <Grid item xs={6}>
                <MusicImport audioPlayerRef={audioRef} notify={notify} setLoading={setLoading} />
              </Grid>
            </Grid>
          </div>
          <RightTab updateHierarchy={updateHierarchy} />
        </div>
      )}
    </div>
  );
}
