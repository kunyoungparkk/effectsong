import { useState, useEffect } from 'react';
import { Grid, FormControl, InputLabel, NativeSelect } from '@mui/material';
import Util from '../Util';
import CoreManager from '../CoreManager';
import { selectedNodeAtom } from '../atom';
import { useAtomValue } from 'jotai';

const SettingsView = () => {
  const [cameraList, setCameraList] = useState<Array<any>>([]);
  const [activeCameraIndex, setActiveCameraIndex] = useState(-1);
  const selectedNode = useAtomValue(selectedNodeAtom);

  useEffect(() => {
    let tempCameraList = [];
    const renderer = CoreManager.getInstance().getRenderer();
    const activeCamera = renderer.getActiveCamera();
    //when camera deleted on attribute panel
    if (!activeCamera) {
      setActiveCameraIndex(-1);
    }
    for (let i = 0; i < renderer.getSceneCount(); i++) {
      const scene = renderer.getSceneAt(i)!;
      for (let j = 0; j < scene.getCameraCount(); j++) {
        const camera = scene.getCameraAt(j)!;
        if (activeCamera && camera.isAliasOf(activeCamera)) {
          setActiveCameraIndex(tempCameraList.length);
        }
        tempCameraList.push(camera.getNode());
      }
    }
    setCameraList(tempCameraList);
  }, [selectedNode]);

  return (
    <Grid
      container
      rowSpacing={2}
      columnSpacing={{ xs: 1, sm: 2, md: 3 }}
      sx={{
        paddingLeft: 2,
        paddingRight: 2,
      }}>
      <Grid item xs={12}>
        <FormControl sx={{ width: '100%' }}>
          <InputLabel variant="standard" htmlFor="active-camera" style={{ color: '#868686' }}>
            Active Camera
          </InputLabel>
          <NativeSelect
            value={activeCameraIndex}
            inputProps={{
              name: 'active-camera',
              id: 'active-camera',
            }}
            style={{ color: 'white' }}
            onChange={(e: React.ChangeEvent<HTMLSelectElement>) => {
              if (!Util.isValidNum(e.target.value)) {
                return;
              }

              const inputIndex = Number(e.target.value);
              let newActiveCamera = cameraList[inputIndex].getCamera();
              CoreManager.getInstance().getRenderer().setActiveCamera(newActiveCamera);
              setActiveCameraIndex(inputIndex);
            }}>
            {cameraList.map((cameraNode, index) => {
              return (
                <option value={index} key={index} style={{ color: 'white', backgroundColor: '#2e2e2e' }}>
                  {cameraNode.getName()}
                </option>
              );
            })}
          </NativeSelect>
        </FormControl>
      </Grid>
    </Grid>
  );
};

export default SettingsView;
