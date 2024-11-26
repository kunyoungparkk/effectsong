import { useEffect, useState } from "react";
import { TextField, Grid } from "@mui/material";
import useUtil from "../Util";

const CameraView = ({ targetNode }) => {
  const Util = useUtil();
  const camera = targetNode?.getCamera();

  const [name, setName] = useState("");
  const [fov, setFov] = useState(0.0);
  const [near, setNear] = useState(0.0);
  const [far, setFar] = useState(0.0);
  const [xMag, setXMag] = useState(0.0);
  const [yMag, setYMag] = useState(0.0);

  useEffect(() => {
    if (!camera) {
      return;
    }
    setName(camera.name);
    setFov(Util.roundToNearestStep(camera.fov));
    setNear(Util.roundToNearestStep(camera.zNear));
    setFar(Util.roundToNearestStep(camera.zFar));
    setXMag(Util.roundToNearestStep(camera.xMag));
    setYMag(Util.roundToNearestStep(camera.yMag));
  }, [targetNode]);

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
            type
          </Grid>
          <Grid item xs={12}>
            <TextField
              type="number"
              variant="standard"
              id="fov-input"
              label="fov"
              value={fov}
              onChange={(e) => {
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
              onChange={(e) => {
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
              onChange={(e) => {
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
          <Grid item xs={6}>
            <TextField
              type="number"
              variant="standard"
              id="xMag-input"
              label="xMag"
              value={xMag}
              onChange={(e) => {
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
              onChange={(e) => {
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
        </Grid>
      );
    }
  };
  return <>{getView()}</>;
};

export default CameraView;
