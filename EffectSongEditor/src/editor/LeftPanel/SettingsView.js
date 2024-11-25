import { useState, useEffect } from "react";
import { Grid, FormControl, InputLabel, NativeSelect } from "@mui/material";

const SettingsView = ({ module }) => {
  const [currentCamera, setCurrentCamera] = useState(
    module?.Renderer?.getInstance().getActiveCamera()
  );

  let cameraList = [];
  const renderer = module.Renderer.getInstance();
  for (let i = 0; i < renderer.getSceneCount(); i++) {
    const scene = renderer.getSceneAt(i);
    for (let j = 0; j < scene.getCameraCount(); j++) {
      const camera = scene.getCameraAt(j);
      cameraList.push(camera.getNode());
    }
  }
  console.log(cameraList);

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
            value={currentCamera?.getNode().getName()}
            inputProps={{
              name: "active-camera",
              id: "active-camera",
            }}
            style={{ color: "white" }}
            onChange={(e) => {
              renderer.setActiveCamera(e.target.value);
              setCurrentCamera(e.target.value);
            }}
          >
            {cameraList.map((cameraNode) => {
              return (
                <option
                  value={cameraNode}
                  key={cameraNode}
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
