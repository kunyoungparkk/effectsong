import { useEffect, useRef, useState } from 'react';

import ShaderSettings from './ScriptEditor/ShaderSettings';
import LeftTab from './LeftPanel/LeftTab';
import RightTab from './RightPanel/RightTab';
import MusicImport from './Import/MusicImport';
import GLTFImport from './Import/GLTFImport';

import CoreManager from './CoreManager';
import * as core from '../core/effectsong-core';

import { hierarchyNodeType } from './common';
import { hierarchyDataAtom, notifyMessageAtom, loadingAtom } from './atom';
import { useSetAtom, useAtom } from 'jotai';

import AudioPlayer from 'react-h5-audio-player';
import 'react-h5-audio-player/lib/styles.css';
import { Grid, Snackbar, Alert, CircularProgress, Backdrop, Typography } from '@mui/material';

export default function Editor() {
  const currentWidth = useRef(1000);
  const currentHeight = useRef(1000);

  const audioRef = useRef<AudioPlayer>(null);
  const canvasRef = useRef<HTMLCanvasElement>(null);
  const canvasDivRef = useRef<HTMLDivElement>(null);

  const [moduleLoaded, setModuleLoaded] = useState<boolean>(false);

  const [notifyMessage, setNotifyMessage] = useAtom(notifyMessageAtom);
  const [loading, setLoading] = useAtom(loadingAtom);

  const setHierarchyData = useSetAtom(hierarchyDataAtom);

  const recursiveWriteNodes = (curNode: core.Node, id: string) => {
    //현재 노드 기록
    let currentData: hierarchyNodeType = {
      id: id,
      name: curNode.getName(),
      children: [],
    };

    //0:scene, 1~: node
    for (let i = 0; i < curNode.getChildrenCount(); i++) {
      let currentChildNode = curNode.getChildAt(i);
      currentData.children.push(recursiveWriteNodes(currentChildNode as core.Node, id + '-' + i));
    }
    return currentData;
  };

  const updateHierarchy = () => {
    let data = [];
    let renderer = CoreManager.getInstance().getRenderer();
    for (let i = 0; i < renderer.getSceneCount(); i++) {
      data.push(recursiveWriteNodes(renderer.getSceneAt(i) as core.Node, i.toString()));
    }
    setHierarchyData(data);
  };

  const onResizeEngine = (width: number, height: number) => {
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
  };

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
        {moduleLoaded && <LeftTab />}
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
                <GLTFImport updateHierarchy={updateHierarchy}/>
              </Grid>
              <Grid item xs={6}>
                <MusicImport audioPlayerRef={audioRef}/>
              </Grid>
            </Grid>
          </div>
          <RightTab updateHierarchy={updateHierarchy} />
        </div>
      )}

      {/*loading popup*/}
      <Backdrop open={loading}>
        <CircularProgress color="primary" />
      </Backdrop>
      {/*notify popup*/}
      <Snackbar
        anchorOrigin={{ vertical: 'top', horizontal: 'center' }}
        open={notifyMessage.open}
        onClose={() => {
          setNotifyMessage({
            open: false,
            success: false,
            message: ''
          });
        }}
        key="notify">
        <Alert severity={notifyMessage.success ? 'success' : 'error'} variant="filled" sx={{ width: '100%' }}>
          {notifyMessage.message}
        </Alert>
      </Snackbar>
    </div>
  );
}
