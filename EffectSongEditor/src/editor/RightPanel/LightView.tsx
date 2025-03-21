import { useEffect, useState, useMemo } from "react";
import {
  TextField,
  Grid,
  FormControl,
  InputLabel,
  NativeSelect,
} from "@mui/material";
import Util from "../Util";
import * as core from '../../core/effectsong-core';
import CoreManager from "../CoreManager";

type lightViewType = {
  targetNode: core.Node | null
}
const LightView = ({ targetNode }: lightViewType) => {
  const light = targetNode?.getLight();
  const module = CoreManager.getInstance().getModule();

  // const [name, setName] = useState("");
  const [color, setColor] = useState<[string, string, string]>(['0', '0', '0']);
  const [intensity, setIntensity] = useState<string>('0.0');
  const [range, setRange] = useState<string>('0.0');
  const [innerConeAngle, setInnerConeAngle] = useState<string>('0.0');
  const [outerConeAngle, setOuterConeAngle] = useState<string>('0.0');
  const [type, setType] = useState("");

  const lightTypes = useMemo(() => ["None", "Directional Light", "Point Light", "Spot Light"], []);

  useEffect(() => {
    if (!light) {
      return;
    }
    // setName(light.name);
    setColor([
      Util.roundToNearestStep(light.color.x).toString(),
      Util.roundToNearestStep(light.color.y).toString(),
      Util.roundToNearestStep(light.color.z).toString(),
    ]);
    setIntensity(Util.roundToNearestStep(light.intensity).toString());
    setRange(Util.roundToNearestStep(light.range).toString());
    setInnerConeAngle(Util.roundToNearestStep(light.innerConeAngle).toString());
    setOuterConeAngle(Util.roundToNearestStep(light.outerConeAngle).toString());
    setType(lightTypes[light.lightType.value]);
  }, [light, lightTypes]);

  const changeColor = (targetColor: [string, string, string]) => {
    if(!light){
      return;
    }
    
    let tempColor: [number, number, number] = [
      Util.isValidNum(targetColor[0]) ? parseFloat(targetColor[0]) : 0.0,
      Util.isValidNum(targetColor[1]) ? parseFloat(targetColor[1]) : 0.0,
      Util.isValidNum(targetColor[2]) ? parseFloat(targetColor[2]) : 0.0
    ];

    light.color = (new module.vec3(...tempColor));
    setColor(targetColor);
  }

  const getView = () => {
    if (!light) {
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
            No Light
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
            Light
          </Grid>
          <Grid item xs={12}>
            <FormControl sx={{ width: "100%" }}>
              <InputLabel
                variant="standard"
                htmlFor="light-type-input"
                style={{ color: "#868686" }}
              >
                Light Type
              </InputLabel>
              <NativeSelect
                value={type}
                inputProps={{
                  name: "light-type-input",
                  id: "light-type-input",
                }}
                style={{ color: "white" }}
                onChange={(e) => {
                  if (e.target.value === "None") {
                    light.lightType = module.LightType.NONE;
                  } else if (e.target.value === "Directional Light") {
                    light.lightType = module.LightType.DIRECTIONAL_LIGHT;
                  } else if (e.target.value === "Point Light") {
                    light.lightType = module.LightType.POINT_LIGHT;
                  } else if (e.target.value === "Spot Light") {
                    light.lightType = module.LightType.SPOT_LIGHT;
                  }
                  setType(e.target.value);
                }}
              >
                {lightTypes.map((lightType) => {
                  return (
                    <option
                      value={lightType}
                      key={lightType}
                      style={{ color: "white", backgroundColor: "#2e2e2e" }}
                    >
                      {lightType}
                    </option>
                  );
                })}
              </NativeSelect>
            </FormControl>
          </Grid>
          <Grid item xs={12}>
            Color
          </Grid>
          <Grid item xs={4}>
            <TextField
              type="number"
              variant="standard"
              id="color-r"
              label="r"
              value={color[0]}
              onChange={(e: React.ChangeEvent<HTMLInputElement | HTMLTextAreaElement>) => {
                changeColor([e.target.value, color[1], color[2]]);
              }}
              InputLabelProps={{
                shrink: true,
                style: { color: "#868686" },
              }}
              inputProps={{ style: { color: "white" } }}
            />
          </Grid>
          <Grid item xs={4}>
            <TextField
              type="number"
              variant="standard"
              id="color-g"
              label="g"
              value={color[1]}
              onChange={(e: React.ChangeEvent<HTMLInputElement | HTMLTextAreaElement>) => {
                changeColor([color[0], e.target.value, color[2]]);
              }}
              InputLabelProps={{
                shrink: true,
                style: { color: "#868686" },
              }}
              inputProps={{ style: { color: "white" } }}
            />
          </Grid>
          <Grid item xs={4}>
            <TextField
              type="number"
              variant="standard"
              id="color-b"
              label="b"
              value={color[2]}
              onChange={(e: React.ChangeEvent<HTMLInputElement | HTMLTextAreaElement>) => {
                changeColor([color[0], color[1], e.target.value]);
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
              id="intensity-input"
              label="intensity"
              value={intensity}
              onChange={(e: React.ChangeEvent<HTMLInputElement | HTMLTextAreaElement>) => {
                if (Util.isValidNum(e.target.value)) {
                  const floatValue = parseFloat(e.target.value);
                  light.intensity = floatValue;
                }
                setIntensity(e.target.value);
              }}
              InputLabelProps={{
                shrink: true,
                style: { color: "#868686" },
              }}
              inputProps={{ style: { color: "white" } }}
            />
          </Grid>
          {type === "Point Light" || type === "Spot Light" ? (
            <Grid item xs={6}>
              <TextField
                type="number"
                variant="standard"
                id="range-input"
                label="range"
                value={range}
                onChange={(e: React.ChangeEvent<HTMLInputElement | HTMLTextAreaElement>) => {
                  if (Util.isValidNum(e.target.value)) {
                    const floatValue = parseFloat(e.target.value);
                    light.range = floatValue;
                  }
                  setRange(e.target.value);
                }}
                InputLabelProps={{
                  shrink: true,
                  style: { color: "#868686" },
                }}
                inputProps={{ style: { color: "white" } }}
              />
            </Grid>
          ) : null}

          {type === "Spot Light" ? (
            <>
              <Grid item xs={6}>
                <TextField
                  type="number"
                  variant="standard"
                  id="innerConeAngle-input"
                  label="innerConeAngle"
                  value={innerConeAngle}
                  onChange={(e: React.ChangeEvent<HTMLInputElement | HTMLTextAreaElement>) => {
                    if (Util.isValidNum(e.target.value)) {
                      const floatValue = parseFloat(e.target.value);
                      light.innerConeAngle = floatValue;
                    }
                    setInnerConeAngle(e.target.value);

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
                  id="outerConeAngle-input"
                  label="outerConeAngle"
                  value={outerConeAngle}
                  onChange={(e: React.ChangeEvent<HTMLInputElement | HTMLTextAreaElement>) => {
                    if (Util.isValidNum(e.target.value)) {
                      const floatValue = parseFloat(e.target.value);
                      light.outerConeAngle = floatValue;
                    }
                    setOuterConeAngle(e.target.value);
                  }}
                  InputLabelProps={{
                    shrink: true,
                    style: { color: "#868686" },
                  }}
                  inputProps={{ style: { color: "white" } }}
                />
              </Grid>
            </>
          ) : null}
        </Grid>
      );
    }
  };
  return getView();
};

export default LightView;
