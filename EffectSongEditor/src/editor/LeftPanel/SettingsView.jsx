import { useState, useEffect } from "react";
import { Grid, FormControl, InputLabel, NativeSelect } from "@mui/material";

const SettingsView = ({ module, targetNode }) => {
  const [cameraList, setCameraList] = useState([]);
  const [activeCameraIndex, setActiveCameraIndex] = useState(-1);

  useEffect(() => {
    let tempCameraList = [];
    const renderer = module.Renderer.getInstance();
    const activeCamera = renderer.getActiveCamera();
    //when camera deleted on attribute panel
    if(!activeCamera){
      setActiveCameraIndex(-1);
    }
    for (let i = 0; i < renderer.getSceneCount(); i++) {
      const scene = renderer.getSceneAt(i);
      for (let j = 0; j < scene.getCameraCount(); j++) {
        const camera = scene.getCameraAt(j);
        if (activeCamera && camera.$$.ptr === activeCamera.$$.ptr) {
          setActiveCameraIndex(tempCameraList.length);
        }
        tempCameraList.push(camera.getNode());
      }
    }
    setCameraList(tempCameraList);
  }, [targetNode]);

  return (
    <Grid
      container
      rowSpacing={2}
      columnSpacing={{ xs: 1, sm: 2, md: 3 }}
      sx={{
        paddingLeft: 2,
        paddingRight: 2,
      }}
    >
      <Grid item xs={12}>
        <FormControl sx={{ width: "100%" }}>
          <InputLabel
            variant="standard"
            htmlFor="active-camera"
            style={{ color: "#868686" }}
          >
            Active Camera
          </InputLabel>
          <NativeSelect
            value={activeCameraIndex}
            inputProps={{
              name: "active-camera",
              id: "active-camera",
            }}
            style={{ color: "white" }}
            onChange={(e) => {
              let newActiveCamera = cameraList[e.target.value].getCamera();
              module.Renderer.getInstance().setActiveCamera(newActiveCamera);
              setActiveCameraIndex(e.target.value);
            }}
          >
            {cameraList.map((cameraNode, index) => {
              return (
                <option
                  value={index}
                  key={index}
                  style={{ color: "white", backgroundColor: "#2e2e2e" }}
                >
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
