import { useState, useEffect } from "react";
import { NativeSelect, TextField, Grid, IconButton } from "@mui/material";
import ScatterPlotIcon from "@mui/icons-material/ScatterPlot";
import CategoryIcon from "@mui/icons-material/Category";
import VisibilityIcon from "@mui/icons-material/Visibility";
import VisibilityOffIcon from "@mui/icons-material/VisibilityOff";
import LightModeIcon from '@mui/icons-material/LightMode';
import HeightIcon from '@mui/icons-material/Height';
import ScriptEditor from "./ScriptEditor";
import useUtil from "../Util";
import Slider from '@mui/material/Slider';

const ShaderSettings = ({ module, onResizeEngine }) => {
  const [scriptOpacity, setScriptOpacity] = useState(0.2);
  const primitiveTypes = [
    "POINTS",
    "LINES",
    "LINE_LOOP",
    "LINE_STRIP",
    "TRIANGLES",
    "TRI_STRIP",
    "TRI_FAN",
  ];
  const [primitiveMode, setPrimitiveMode] = useState(0);
  const [scriptVisible, setScriptVisible] = useState(true);
  const [vertexCount, setVertexCount] = useState(0);
  const [diffuseIBLIntensity, setDiffuseIBLIntensity] = useState(0.0);
  const [width, setWidth] = useState(0);
  const [height, setHeight] = useState(0);
  const Util = useUtil();

  const [vertexShader, setVertexShader] = useState(`//shader art sample
#define PI 3.14159
#define NUM_SEGMENTS 51.0
#define NUM_POINTS (NUM_SEGMENTS * 2.0)
#define STEP 5.0

vec3 hsv2rgb(vec3 c) {
    c = vec3(c.x, clamp(c.yz, 0.0, 1.0));
    vec4 K = vec4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);
    vec3 p = abs(fract(c.xxx + K.xyz) * 6.0 - K.www);
    return c.z * mix(K.xxx, clamp(p - K.xxx, 0.0, 1.0), c.y);
}

void main() {
    float num = mod(vertexId, 2.0);
    float point = mod(floor(vertexId / 2.0) + mod(vertexId, 2.0) * STEP, NUM_SEGMENTS);
    float count = floor(vertexId / NUM_POINTS);
    float offset = count * 0.02;
    float angle = point * PI * 2.0 / NUM_SEGMENTS - offset;
    float spread = 0.02;
    float off = 0.1;
    float speed = count * 0.004;
    float snd = 1.0;

    if (num < 0.9) {
        off = 0.2;
    } else {
        off = 0.1;
    }
        snd *= (
            texture(sound, vec2(off + spread * 0., speed)).r +
            texture(sound, vec2(off + spread * 1., speed)).r +
            texture(sound, vec2(off + spread * 2., speed)).r) / 3.;
    
    float leftTarget =  texture(sound, vec2(off, 0.0)).r;
    float rightTarget =  texture(sound2, vec2(off, 0.0)).r;
    
    float rPulse = pow(snd, 5.0);
    float radius = count * 0.02 + rPulse * 0.4; 

    float rotationSpeed = num < 0.9 ? 1.0 : -1.0; 
    float c = cos(angle + time * rotationSpeed) * radius;
    float s = sin(angle + time * rotationSpeed) * radius;

    vec2 aspect = vec2(1, resolution.x / resolution.y);
    vec2 xy = vec2(c, s);

    gl_Position = vec4(xy * aspect, 0, 1);
    gl_Position.x = num < 0.9 
        ? gl_Position.x * 0.5 - 0.5 
        : gl_Position.x * 0.5 + 0.5; 
    
    gl_PointSize = 2.0 + length(xy) * 20. * resolution.x / 1600.0;

    float hue = time * 0.03 + count * 1.001 + (num < 0.9 ? 0.0 : 0.5);
    float cPulse = pow(rPulse, 2.0);
    float invCPulse = 1.0 - cPulse;
    vec4 color = vec4(hsv2rgb(vec3(hue, invCPulse, 1.0)), 1.0);
    v_color = mix(color, background, radius - cPulse);
}
`);

  useEffect(() => {
    if (!module) {
      return;
    }
    let success = module.ArtShader.getInstance().setVertexShader(vertexShader);
    console.log('compile: ' + success);
  }, [vertexShader]);

  useEffect(() => {
    if (!module) {
      return;
    }
    let artShader = module.ArtShader.getInstance();
    setPrimitiveMode(artShader.getPrimitiveMode());
    setVertexCount(artShader.getVertexCount());
    artShader.setVertexShader(vertexShader);

    const renderer = module.Renderer.getInstance();
    setDiffuseIBLIntensity(renderer.getDiffuseIBLIntensity());
    setWidth(renderer.getWidth());
    setHeight(renderer.getHeight());
  }, [module]);
  return (
    <div>
      <Grid
        container
        sx={{
          width: "calc(100% -  650px)",
          height: "40px",
          backgroundColor: "white",
          textAlign: "right",
          paddingLeft: "15px",
          paddingTop: "3px",
          float: "left",
        }}
      >
        <Grid item xs={0.5} key="script-visible-button">
          <IconButton
            aria-label="shader-visible"
            onClick={() => {
              setScriptVisible(!scriptVisible);
            }}
            style={{ paddingTop: 4, color: "#868686" }}
          >
            {scriptVisible ? (
              <VisibilityIcon sx={{ color: "#868686" }} />
            ) : (
              <VisibilityOffIcon sx={{ color: "#868686" }} />
            )}
          </IconButton>
        </Grid>
        <Grid item xs={1.0} key="script-visible-opacity">
          <Slider
            size="small"
            value={scriptOpacity}
            aria-label="script-visible-opacity"
            step={0.01}
            min={0.0}
            max={1.0}
            onChange={(e) => { setScriptOpacity(e.target.value); }}
            style={{ paddingTop: 17, color: "#868686" }}
          />
        </Grid>
        <Grid item xs={0.5} key="primitive-select-icon">
          <CategoryIcon sx={{ fontSize: "20px", paddingTop: "7px", color: "#868686" }} />
        </Grid>
        <Grid item xs={1.3} key="primitive-select">
          <NativeSelect
            value={primitiveMode}
            inputProps={{
              name: "Primitive",
              id: "primitive-select",
            }}
            onChange={(e) => {
              module.ArtShader.getInstance().setPrimitiveMode(e.target.value);
              setPrimitiveMode(e.target.value);
            }}
            style={{ color: "#868686" }}
          >
            {primitiveTypes.map((value, index) => {
              return (
                <option value={index} key={index}>
                  {value}
                </option>
              );
            })}
          </NativeSelect>
        </Grid>
        <Grid item xs={0.5} key="vertex-count-icon">
          <ScatterPlotIcon sx={{ fontSize: "20px", paddingTop: "7px", color: "#868686" }} />
        </Grid>
        <Grid item xs={0.1} key="space0"/>
        <Grid item xs={1.0} key="vertex-count">
          <TextField
            type="number"
            variant="standard"
            value={vertexCount}
            onChange={(e) => {
              if (Util.isValidNum(e.target.value)) {
                let intValue = parseInt(e.target.value);
                intValue = Math.min(1000000, intValue);
                intValue = Math.max(0, intValue);
                module.ArtShader.getInstance().setVertexCount(intValue);
                setVertexCount(intValue);
              } else {
                module.ArtShader.getInstance().setVertexCount(0);
                setVertexCount(e.target.value);
              }
            }}
            inputProps={{ style: { color: "#868686" } }}
          />
        </Grid>
        <Grid item xs={0.5} key="diffuse-ibl-intensity-icon">
          <LightModeIcon sx={{ fontSize: "20px", paddingTop: "5px", color: "#868686" }} />
        </Grid>
        <Grid item xs={0.1} key="space1"/>
        <Grid item xs={1.0} key="diffuse-ibl-intensity">
          <TextField
            type="number"
            variant="standard"
            value={diffuseIBLIntensity}
            onChange={(e) => {
              if (Util.isValidNum(e.target.value)) {
                const floatValue = parseFloat(e.target.value);
                module.Renderer.getInstance().setDiffuseIBLIntensity(floatValue);
                setDiffuseIBLIntensity(floatValue);
              } else {
                module.Renderer.getInstance().setDiffuseIBLIntensity(0.0);
                setDiffuseIBLIntensity(e.target.value);
              }
            }}
            inputProps={{ style: { color: "#868686" } }}
          />
        </Grid>
        <Grid item xs={0.5} key="width-icon">
          <HeightIcon sx={{ fontSize: "20px", paddingLeft: "10px", color: "#868686", transform: 'rotate(90deg)' }} />
        </Grid>
        <Grid item xs={0.1} key="space2"/>
        <Grid item xs={1.0} key="width-size">
          <TextField
            type="number"
            variant="standard"
            value={width}
            onChange={(e) => {
              if (Util.isValidNum(e.target.value)) {
                let intValue = parseInt(e.target.value);
                intValue = Math.min(10000, intValue);
                intValue = Math.max(0, intValue);
                module.Renderer.getInstance().resize(intValue, height);
                setWidth(intValue);
                onResizeEngine(intValue, height);
              } else {
                module.Renderer.getInstance().resize(0, height);
                setWidth(e.target.value);
                onResizeEngine(0, height);
              }
            }}
            inputProps={{ style: { color: "#868686" } }}
          />
        </Grid>

        <Grid item xs={0.5} key="height-icon">
          <HeightIcon sx={{ fontSize: "20px", paddingTop: "7px", color: "#868686" }} />
        </Grid>
        <Grid item xs={0.1} key="space3"/>
        <Grid item xs={1.0} key="height-size">
          <TextField
            type="number"
            variant="standard"
            value={height}
            onChange={(e) => {
              if (Util.isValidNum(e.target.value)) {
                let intValue = parseInt(e.target.value);
                intValue = Math.min(10000, intValue);
                intValue = Math.max(0, intValue);
                module.Renderer.getInstance().resize(width, intValue);
                setHeight(intValue);
                onResizeEngine(width, intValue);
              } else {
                module.Renderer.getInstance().resize(width, 0);
                setHeight(e.target.value);
                onResizeEngine(width, 0);
              }
            }}
            inputProps={{ style: { color: "#868686" } }}
          />
        </Grid>
      </Grid>

      {scriptVisible ? (
        <ScriptEditor
          vertexShader={vertexShader}
          setVertexShader={setVertexShader}
          opacity={scriptOpacity}
        ></ScriptEditor>
      ) : (
        <></>
      )}
    </div>
  );
};

export default ShaderSettings;
