import { useEffect, useState, useMemo } from "react";
import { TextField, Grid, FormControl, InputLabel, NativeSelect } from "@mui/material";
import Util from "../Util";
import * as core from '../../core/effectsong-core'

type cameraView = {
  module: core.MainModule,
  targetNode: core.Node | null
}
const CameraView = ({ module, targetNode }: cameraView) => {
  const camera = targetNode?.getCamera();

  //const [name, setName] = useState("");
  const [fov, setFov] = useState<number | string>();
  const [near, setNear] = useState<number | string>();
  const [far, setFar] = useState<number | string>();
  const [xMag, setXMag] = useState<number | string>();
  const [yMag, setYMag] = useState<number | string>();
  const [type, setType] = useState('');

  const cameraTypes = useMemo(()=>['Perspective', 'Orthographic'], []);

  useEffect(() => {
    if (!camera) {
      return;
    }
    //setName(camera.name);
    setFov(Util.roundToNearestStep(camera.fov));
    setNear(Util.roundToNearestStep(camera.zNear));
    setFar(Util.roundToNearestStep(camera.zFar));
    setXMag(Util.roundToNearestStep(camera.xMag));
    setYMag(Util.roundToNearestStep(camera.yMag));
    setType(cameraTypes[camera.projectionType.value]);
  }, [camera, cameraTypes]);

  const getView = () => {
    if (!camera) {
      return (
        <Grid
          container
          rowSpacing={2}
          columnSpacing={{ xs: 1, sm: 2, md: 3 }}
          sx={{
            padding: 1,
            color: "#868686",
          }}
        >
          <Grid item xs={12}>
            No Camera
          </Grid>
        </Grid>
      );
    } else {
      return (
        <Grid
          container
          rowSpacing={2}
          columnSpacing={{ xs: 1, sm: 2, md: 3 }}
          sx={{
            padding: 1,
            color: "#868686",
          }}
        >
          <Grid item xs={12}>
            Camera
          </Grid>
          <Grid item xs={12}>
            <FormControl sx={{ width: "100%" }}>
              <InputLabel
                variant="standard"
                htmlFor="camera-projection-type-input"
                style={{ color: "#868686" }}
              >
                Projection Type
              </InputLabel>
              <NativeSelect
                value={type}
                inputProps={{
                  name: "camera-projection-type-input",
                  id: "camera-projection-type-input",
                }}
                style={{ color: "white" }}
                onChange={(e) => {
                  if (e.target.value === "Perspective") {
                    camera.projectionType = module.ProjectionType.PERSPECTIVE;
                  } else {
                    camera.projectionType = module.ProjectionType.ORTHOGRAPHIC;
                  }
                  setType(e.target.value);
                }}
              >
                {cameraTypes.map((cameraType) => {
                  return (
                    <option
                      value={cameraType}
                      key={cameraType}
                      style={{ color: "white", backgroundColor: "#2e2e2e" }}
                    >
                      {cameraType}
                    </option>
                  );
                })}
              </NativeSelect>
            </FormControl>
          </Grid>
          <Grid item xs={12}>
            <TextField
              type="number"
              variant="standard"
              id="fov-input"
              label="fov"
              value={fov}
              onChange={(e: React.ChangeEvent<HTMLInputElement | HTMLTextAreaElement>) => {
                if (Util.isValidNum(e.target.value)) {
                  const floatValue = parseFloat(e.target.value);
                  camera.fov = floatValue;
                  setFov(floatValue);
                } else {
                  setFov(e.target.value);
                }
              }}
              InputLabelProps={{
                shrink: true,
                style: { color: "#868686" },
              }}
              inputProps={{ style: { color: "white" } }}
            />
          </Grid>
          <Grid item xs={6}>
            <TextField
              type="number"
              variant="standard"
              id="near-input"
              label="near"
              value={near}
              onChange={(e: React.ChangeEvent<HTMLInputElement | HTMLTextAreaElement>) => {
                if (Util.isValidNum(e.target.value)) {
                  const floatValue = parseFloat(e.target.value);
                  camera.zNear = floatValue;
                  setNear(floatValue);
                } else {
                  setNear(e.target.value);
                }
              }}
              InputLabelProps={{
                shrink: true,
                style: { color: "#868686" },
              }}
              inputProps={{ style: { color: "white" } }}
            />
          </Grid>
          <Grid item xs={6}>
            <TextField
              type="number"
              variant="standard"
              id="far-input"
              label="far"
              value={far}
              onChange={(e: React.ChangeEvent<HTMLInputElement | HTMLTextAreaElement>) => {
                if (Util.isValidNum(e.target.value)) {
                  const floatValue = parseFloat(e.target.value);
                  camera.zFar = floatValue;
                  setFar(floatValue);
                } else {
                  setFar(e.target.value);
                }
              }}
              InputLabelProps={{
                shrink: true,
                style: { color: "#868686" },
              }}
              inputProps={{ style: { color: "white" } }}
            />
          </Grid>
          {type === cameraTypes[1] ?
            <>
              <Grid item xs={6}>
                <TextField
                  type="number"
                  variant="standard"
                  id="xMag-input"
                  label="xMag"
                  value={xMag}
                  onChange={(e: React.ChangeEvent<HTMLInputElement | HTMLTextAreaElement>) => {
                    if (Util.isValidNum(e.target.value)) {
                      const floatValue = parseFloat(e.target.value);
                      camera.xMag = floatValue;
                      setXMag(floatValue);
                    } else {
                      setXMag(e.target.value);
                    }
                  }}
                  InputLabelProps={{
                    shrink: true,
                    style: { color: "#868686" },
                  }}
                  inputProps={{ style: { color: "white" } }}
                />
              </Grid>
              <Grid item xs={6}>
                <TextField
                  type="number"
                  variant="standard"
                  id="yMag-input"
                  label="yMag"
                  value={yMag}
                  onChange={(e: React.ChangeEvent<HTMLInputElement | HTMLTextAreaElement>) => {
                    if (Util.isValidNum(e.target.value)) {
                      const floatValue = parseFloat(e.target.value);
                      camera.yMag = floatValue;
                      setYMag(floatValue);
                    } else {
                      setYMag(e.target.value);
                    }
                  }}
                  InputLabelProps={{
                    shrink: true,
                    style: { color: "#868686" },
                  }}
                  inputProps={{ style: { color: "white" } }}
                />
              </Grid>
            </>
            : <></>}

        </Grid>
      );
    }
  };
  return <>{getView()}</>;
};

export default CameraView;
