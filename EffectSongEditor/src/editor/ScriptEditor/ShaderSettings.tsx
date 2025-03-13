import { useState, useEffect, useRef } from 'react';
import {
  NativeSelect,
  TextField,
  Grid,
  IconButton,
  Button,
  Modal,
  Typography,
  Box,
  Menu,
  MenuItem,
} from '@mui/material';
import ScatterPlotIcon from '@mui/icons-material/ScatterPlot';
import CategoryIcon from '@mui/icons-material/Category';
import VisibilityIcon from '@mui/icons-material/Visibility';
import VisibilityOffIcon from '@mui/icons-material/VisibilityOff';
import LightModeIcon from '@mui/icons-material/LightMode';
import HeightIcon from '@mui/icons-material/Height';
import HelpIcon from '@mui/icons-material/Help';
import ScriptEditor from './ScriptEditor';
import Util from '../Util';
import Slider from '@mui/material/Slider';
import CoreManager from '../CoreManager';
// import PlayArrowIcon from '@mui/icons-material/PlayArrow';
import AutoAwesomeIcon from '@mui/icons-material/AutoAwesome';
import DeveloperModeIcon from '@mui/icons-material/DeveloperMode';
import { loadingAtom } from '../atom';
import { useSetAtom } from 'jotai';

type ShaderSettingsType = {
  onResizeEngine: (width: number, height: number) => void;
};
const ShaderSettings = ({ onResizeEngine }: ShaderSettingsType) => {
  const DEFAULT_SHADER = `//shader art sample
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
    float off = 0.01;
    float speed = count * 0.004;
    float snd = 1.0;

    if (isStereo && num > 0.9) {
        snd *= (
            texture(sound2, vec2(off + spread * 0., speed)).r +
            texture(sound2, vec2(off + spread * 1., speed)).r +
            texture(sound2, vec2(off + spread * 2., speed)).r) / 3.;
    } else {
        snd *= (
            texture(sound, vec2(off + spread * 0., speed)).r +
            texture(sound, vec2(off + spread * 1., speed)).r +
            texture(sound, vec2(off + spread * 2., speed)).r) / 3.;
    }
    
    float rPulse = pow(snd, 5.0);
    float radius = count * 0.02 + rPulse * 0.4; 

    float rotationSpeed = num < 0.9 ? 1.0 : -1.0; 
    float c = cos(angle + time * rotationSpeed) * radius;
    float s = sin(angle + time * rotationSpeed) * radius;

    vec2 aspect = vec2(1, resolution.x / resolution.y);
    vec2 xy = vec2(c, s);

    gl_Position = vec4(xy * aspect, 0, 1);
    gl_Position.x = num < 0.9 
        ? gl_Position.x * 0.5 - 0.4
        : gl_Position.x * 0.5 + 0.4; 
    
    gl_PointSize = 2.0 + length(xy) * 20. * resolution.x / 1600.0;

    float hue = time * 0.03 + count * 1.001 + (num < 0.9 ? 0.0 : 0.5);
    float cPulse = pow(rPulse, 2.0);
    float invCPulse = 1.0 - cPulse;
    vec4 color = vec4(hsv2rgb(vec3(hue, invCPulse, 1.0)), 1.0);
    v_color = mix(color, background, radius - cPulse);
}
`;
  const soundTextureVS = ` void main() {
  float across = floor(sqrt(vertexCount));
  float down = floor(vertexCount / across);
  
  float x = mod(vertexId, across);
  float y = floor(vertexId / across);
  
  float u = x / across;
  float v = y / across;
  
  vec2 xy = vec2(u * 2.0 - 1.0, v * 2.0 - 1.0);
  gl_Position = vec4(xy, 0, 1);
  gl_PointSize = max(0.1, ceil(resolution.x / across));
  
  float s;
  if(!isStereo || u<0.5){
    s = texture(sound, vec2(u * 2.0, v)).r;
  }else{
    s = texture(sound2, vec2((u - 0.5) * 2.0, v)).r;
  }  
  v_color = vec4(s, s, s, 1.0);
}
`;
  const primitiveTypes = ['POINTS', 'LINES', 'LINE_LOOP', 'LINE_STRIP', 'TRIANGLES', 'TRI_STRIP', 'TRI_FAN'];

  const [scriptOpacity, setScriptOpacity] = useState(0.2);

  const [primitiveMode, setPrimitiveMode] = useState(0);
  const [reqPrimitiveMode, setReqPrimitiveMode] = useState<0 | 1 | 4>(0);

  const [scriptVisible, setScriptVisible] = useState(true);
  const [vertexCount, setVertexCount] = useState<string>('0');
  const [diffuseIBLIntensity, setDiffuseIBLIntensity] = useState<string>('0');
  const [width, setWidth] = useState<string>('0');
  const [height, setHeight] = useState<string>('0');

  const [helpModalOpen, setHelpModalOpen] = useState(false);
  const [guideModalOpen, setGuideModalOpen] = useState(false);

  const [vertexShader, setVertexShader] = useState(DEFAULT_SHADER);
  const lastCompiledShader = useRef(DEFAULT_SHADER);
  //const [targetShaderIndex, setTargetShaderIndex] = useState(0);

  const [aiRequest, setAIRequest] = useState('');
  const [receivingCode, setReceivingCode] = useState(false);

  //menu item
  const [menuAnchorElement, setMenuAnchorElement] = useState<HTMLElement | null>(null);
  const devMenuOpen = Boolean(menuAnchorElement);

  const setLoading = useSetAtom(loadingAtom);

  const coreManager = CoreManager.getInstance();

  useEffect(() => {
    let artShader = coreManager.getArtShader();
    setPrimitiveMode(artShader.getPrimitiveMode());
    setVertexCount(artShader.getVertexCount().toString());

    compileShader(DEFAULT_SHADER);

    const renderer = coreManager.getRenderer();
    setDiffuseIBLIntensity(renderer.getDiffuseIBLIntensity().toString());
    setWidth(renderer.getWidth().toString());
    setHeight(renderer.getHeight().toString());
  }, []);

  const compileShader = (shader: string) => {
    let success = coreManager.getArtShader().setVertexShader(shader);
    if (success) {
      lastCompiledShader.current = shader;
      console.log(success);
    } else {
      console.log(success);
    }
  };

  const generateAIShader = (mode: number, request: string) => {
    setLoading(true);
    setVertexShader("");
    setScriptVisible(true);

    // Initialize the EventSource, listening for server updates
    const url = `http://localhost:7777/stream?primitiveMode=${primitiveTypes[mode]}&request=${request}`;
    const eventSource = new EventSource(url);
    let code = '';

    eventSource.onopen = () => {
      console.log('SSE connected');
      setReceivingCode(true);
      setLoading(true);
    };

    eventSource.onmessage = function (event) {
      if (event.data.includes('<ES_AI_ERROR>')) {
        //TODO: error
      } else {
        const currentData = event.data.replace(/\\n/g, '\n');
        code += currentData;
        setVertexShader(code);
      }
    };

    eventSource.onerror = function (event) {
      console.log('SSE closed:  ', event);
      eventSource.close();
      // setScriptVisible(false);

      setVertexShader(code);
      setPrimitiveMode(mode);
      coreManager.getArtShader().setPrimitiveMode(mode);
      compileShader(code);

      setReceivingCode(false);
      setLoading(false);
      return;
    };
  };

  return (
    <div>
      <Grid
        container
        sx={{
          width: 'calc(100% -  650px)',
          height: '40px',
          backgroundColor: 'white',
          textAlign: 'center',
          paddingLeft: '10px',
          paddingTop: '3px',
          float: 'left',
        }}>
        <Grid item xs={0.5} key="script-visible-button">
          <IconButton
            aria-label="shader-visible"
            onClick={() => {
              setScriptVisible(!scriptVisible);
            }}
            style={{ paddingTop: 4, color: '#868686' }}>
            {scriptVisible ? (
              <VisibilityIcon sx={{ color: '#868686' }} />
            ) : (
              <VisibilityOffIcon sx={{ color: '#868686' }} />
            )}
          </IconButton>
        </Grid>
        <Grid item xs={0.3} key="space" />
        <Grid item xs={1.0} key="script-visible-opacity">
          <Slider
            size="small"
            value={scriptOpacity}
            aria-label="script-visible-opacity"
            step={0.01}
            min={0.0}
            max={1.0}
            onChange={(e: Event, value: number | number[]) => {
              if (!Array.isArray(value)) {
                setScriptOpacity(value);
              }
            }}
            style={{ paddingTop: 17, color: '#868686' }}
          />
        </Grid>
        <Grid item xs={0.25} key="space0" />
        <Grid item xs={0.5} key="primitive-select-icon">
          <CategoryIcon sx={{ fontSize: '20px', paddingTop: '7px', color: '#868686' }} />
        </Grid>
        <Grid item xs={1.3} key="primitive-select">
          <NativeSelect
            value={primitiveMode}
            inputProps={{
              name: 'Primitive',
              id: 'primitive-select',
            }}
            onChange={(e: React.ChangeEvent<HTMLSelectElement>) => {
              coreManager.getArtShader().setPrimitiveMode(parseInt(e.target.value));
              setPrimitiveMode(parseInt(e.target.value));
            }}
            style={{ color: '#868686' }}>
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
          <ScatterPlotIcon sx={{ fontSize: '20px', paddingTop: '7px', color: '#868686' }} />
        </Grid>
        <Grid item xs={0.1} key="space1" />
        <Grid item xs={1.0} key="vertex-count">
          <TextField
            type="number"
            variant="standard"
            value={vertexCount}
            onChange={(e: React.ChangeEvent<HTMLInputElement | HTMLTextAreaElement>) => {
              if (Util.isValidNum(e.target.value)) {
                let intValue = parseInt(e.target.value);
                intValue = Math.min(1000000, intValue);
                intValue = Math.max(0, intValue);
                coreManager.getArtShader().setVertexCount(intValue);
              } else {
                coreManager.getArtShader().setVertexCount(0);
              }
              setVertexCount(e.target.value);
            }}
            inputProps={{ style: { color: '#868686' } }}
          />
        </Grid>
        <Grid item xs={0.5} key="diffuse-ibl-intensity-icon">
          <LightModeIcon sx={{ fontSize: '20px', paddingTop: '5px', color: '#868686' }} />
        </Grid>
        <Grid item xs={0.1} key="space2" />
        <Grid item xs={1.0} key="diffuse-ibl-intensity">
          <TextField
            type="number"
            variant="standard"
            value={diffuseIBLIntensity}
            onChange={(e: React.ChangeEvent<HTMLInputElement | HTMLTextAreaElement>) => {
              if (Util.isValidNum(e.target.value)) {
                const floatValue = parseFloat(e.target.value);
                coreManager.getRenderer().setDiffuseIBLIntensity(floatValue);
              } else {
                coreManager.getRenderer().setDiffuseIBLIntensity(0.0);
              }
              setDiffuseIBLIntensity(e.target.value);
            }}
            inputProps={{ style: { color: '#868686' } }}
          />
        </Grid>
        <Grid item xs={0.5} key="width-icon">
          <HeightIcon
            sx={{
              fontSize: '20px',
              paddingLeft: '10px',
              color: '#868686',
              transform: 'rotate(90deg)',
            }}
          />
        </Grid>
        <Grid item xs={0.1} key="space3" />
        <Grid item xs={1.0} key="width-size">
          <TextField
            type="number"
            variant="standard"
            value={width}
            onChange={(e: React.ChangeEvent<HTMLInputElement | HTMLTextAreaElement>) => {
              const heightValue = coreManager.getRenderer().getHeight();
              if (Util.isValidNum(e.target.value)) {
                let newWidth = parseInt(e.target.value);
                newWidth = Math.min(10000, newWidth);
                newWidth = Math.max(0, newWidth);

                coreManager.getRenderer().resize(newWidth, heightValue);
                onResizeEngine(newWidth, heightValue);
              } else {
                coreManager.getRenderer().resize(0, heightValue);
                onResizeEngine(0, heightValue);
              }
              setWidth(e.target.value);
            }}
            inputProps={{ style: { color: '#868686' } }}
          />
        </Grid>
        <Grid item xs={0.5} key="height-icon">
          <HeightIcon sx={{ fontSize: '20px', paddingTop: '7px', color: '#868686' }} />
        </Grid>
        <Grid item xs={0.1} key="space4" />
        <Grid item xs={1.0} key="height-size">
          <TextField
            type="number"
            variant="standard"
            value={height}
            onChange={(e: React.ChangeEvent<HTMLInputElement | HTMLTextAreaElement>) => {
              const widthValue = coreManager.getRenderer().getWidth();
              if (Util.isValidNum(e.target.value)) {
                let newHeight = parseInt(e.target.value);
                newHeight = Math.min(10000, newHeight);
                newHeight = Math.max(0, newHeight);

                coreManager.getRenderer().resize(widthValue, newHeight);
                onResizeEngine(widthValue, newHeight);
              } else {
                coreManager.getRenderer().resize(widthValue, 0);
                onResizeEngine(widthValue, 0);
              }
              setHeight(e.target.value);
            }}
            inputProps={{ style: { color: '#868686' } }}
          />
        </Grid>
        <Grid item xs={0.5} key="space5" />
        <Grid item xs={1.0} key="help">
          <Button
            size="small"
            variant="contained"
            startIcon={<HelpIcon />}
            sx={{ width: '100%' }}
            onClick={() => {
              setHelpModalOpen(true);
            }}>
            help
          </Button>
        </Grid>
        <Grid item xs={0.25} key="space6"></Grid>

      </Grid>

      <Grid
        container
        sx={{
          width: 'calc(100% -  650px)',
          height: '40px',
          backgroundColor: '#eeeeee',
          textAlign: 'center',
          float: 'left',
        }}>
        {/* another line */}
        <Grid item xs={0.1} sx={{ backgroundColor: "#000027" }}>
        </Grid>
        <Grid item xs={0.1} key="space12" />
        <Grid item xs={1.2}>
          <NativeSelect
            value={reqPrimitiveMode}
            inputProps={{
              name: 'Primitive',
              id: 'primitive-select',
            }}
            onChange={(e: React.ChangeEvent<HTMLSelectElement>) => {
              setReqPrimitiveMode(parseInt(e.target.value) as typeof reqPrimitiveMode);
            }}
            style={{
              color: '#868686',
              paddingTop: '4px'
            }}>
            <option value={0} key={0}>
              {primitiveTypes[0]}
            </option>
            <option value={1} key={1}>
              {primitiveTypes[1]}
            </option>
            <option value={4} key={4}>
              {primitiveTypes[4]}
            </option>
          </NativeSelect>
        </Grid>
        <Grid item xs={0.1} key="space11" />
        <Grid item xs={6.8}>
          <TextField
            id="standard-basic"
            variant="standard"
            value={aiRequest}
            placeholder="Explain the visual effect you want"
            onChange={(e) => {
              setAIRequest(e.target.value);
            }}
            sx={{ width: '100%', height: '50%', 
              paddingTop: '4px' }}
          />
        </Grid>
        <Grid item xs={0.2} key="space8" />
        <Grid item xs={2}>
          <Button
            size="small"
            variant="contained"
            startIcon={<AutoAwesomeIcon />}
            sx={{
              width: '100%', color: 'white', marginTop: 0.4
            }}
            onClick={() => {
              if (aiRequest.length > 0) {
                generateAIShader(reqPrimitiveMode, aiRequest);
              }
            }}>
            Generate
          </Button>
        </Grid>
        <Grid item xs={0.25} key="space9" />
        <Grid item xs={1.0}>
          <Button
            id="devButton"
            size="small"
            variant="outlined"
            startIcon={<DeveloperModeIcon />}
            sx={{ width: '100%', color: '#868686', marginTop: 0.4 }}
            onClick={(event) => {
              setMenuAnchorElement(event.currentTarget);
            }}>
            DEV
          </Button>
        </Grid>
        <Grid item xs={0.25} key="space10" />
      </Grid>

      <Menu
        id="dev-menu"
        anchorEl={menuAnchorElement}
        open={devMenuOpen}
        onClose={() => {
          setMenuAnchorElement(null);
        }}
        MenuListProps={{
          'aria-labelledby': 'devButton',
        }}>
        <MenuItem
          onClick={() => {
            compileShader(vertexShader);
            setMenuAnchorElement(null);
          }}>
          Compile
        </MenuItem>
        <MenuItem
          onClick={() => {
            setGuideModalOpen(true);
            setMenuAnchorElement(null);
          }}>
          Show Developer Guide
        </MenuItem>
        <MenuItem
          onClick={() => {
            setMenuAnchorElement(null);
            coreManager.getArtShader().setPrimitiveMode(0);
            coreManager.getArtShader().setVertexShader(soundTextureVS);
          }}>
          Show Sound Texture
        </MenuItem>
        <MenuItem
          onClick={() => {
            setMenuAnchorElement(null);
            coreManager.getArtShader().setPrimitiveMode(primitiveMode);
            compileShader(lastCompiledShader.current);
          }}>
          Hide Sound Texture
        </MenuItem>
      </Menu>

      <Modal
        open={guideModalOpen}
        onClose={() => {
          setGuideModalOpen(false);
        }}
        aria-labelledby="guide-modal"
        aria-describedby="guide-modal-description">
        <Box
          sx={{
            position: 'absolute',
            top: '50%',
            left: '50%',
            transform: 'translate(-50%, -50%)',
            width: 400,
            bgcolor: 'background.paper',
            border: '2px solid #000',
            boxShadow: 24,
            p: 4,
          }}>
          <Typography id="guide-title" variant="h5" component="h2">
            Shader Parameter Guide
          </Typography>
          <Typography id="guide-subtitle" component="h5">
            (GLSL 3.0 vertex shader)
          </Typography>
          <Typography sx={{ mt: 2 }}>float vertexId : current vertexId (0 ~ vertexCount - 1)</Typography>
          <Typography sx={{ mt: 2 }}>float volume : current volume</Typography>
          <Typography sx={{ mt: 2 }}>vec2 resolution : shader art texture resolution (maybe 2048, 2048)</Typography>
          <Typography sx={{ mt: 2 }}>vec4 background : background color</Typography>
          <Typography sx={{ mt: 2 }}>float time : current music time</Typography>
          <Typography sx={{ mt: 2 }}>float vertexCount : total vertex counts</Typography>
          <Typography sx={{ mt: 2 }}>sampler2D sound : left sound texture, use r channel</Typography>
          <Typography sx={{ mt: 2 }}>sampler2D sound2 : right sound texture, use r channel</Typography>
          <Typography sx={{ mt: 2 }}>bool isStereo : if stereo, it is true. if mono, it is false</Typography>
          <hr />
          <Typography sx={{ mt: 2 }}>vec4 v_color : output color</Typography>
          <Typography sx={{ mt: 2 }}>vec4 gl_Position : output position (NDC)</Typography>
          <Typography sx={{ mt: 2 }}>float gl_PointSize : output point size</Typography>
        </Box>
      </Modal>

      <Modal
        open={helpModalOpen}
        onClose={() => {
          setHelpModalOpen(false);
        }}
        aria-labelledby="help-modal"
        aria-describedby="help-modal-description">
        <Box
          sx={{
            position: 'absolute',
            top: '50%',
            left: '50%',
            transform: 'translate(-50%, -50%)',
            width: 400,
            bgcolor: 'background.paper',
            border: '2px solid #000',
            boxShadow: 24,
            p: 4,
          }}>
          <Typography id="help-title" variant="h5" component="h2">
            EffectSong Help
          </Typography>
          <Typography id="help-subtitle" component="h5">
            (How to use EffectSong)
          </Typography>
          <Typography sx={{ mt: 2 }}>
            1. The top UI bar displays, from left to right:{' '}
            <b>code visualization, primitive mode, vertex count, diffuse light intensity, width, and height</b>.
          </Typography>
          <Typography sx={{ mt: 2 }}>
            2. In the second UI row, enter a description of the visual effect you want in the text input box. Based on
            the <b>primitive mode</b> you selected, the AI will generate the appropriate shader code. Click the{' '}
            <b>Generate</b> button to let the AI create it for you.
          </Typography>
          <Typography sx={{ mt: 2 }}>
            3. Use the <b>top-right UI panel</b> to upload your own <b>music and glTF/glb model</b> for further
            customization.
          </Typography>
          <Typography sx={{ mt: 2 }}>
            4. If you're an <b>advanced user</b> modify the GLSL code yourself and compile it using the{' '}
            <b>DEV button menu</b>.
          </Typography>
          <Typography sx={{ mt: 2 }}>5. Enjoy!</Typography>
        </Box>
      </Modal>

      {scriptVisible ? (
        <ScriptEditor
          vertexShader={vertexShader}
          setVertexShader={setVertexShader}
          opacity={scriptOpacity}
          receivingCode={receivingCode}
        //setPrimitiveMode={setPrimitiveMode}
        //setVertexCount={setVertexCount}
        //targetShaderIndex={targetShaderIndex}
        //setTargetShaderIndex={setTargetShaderIndex}
        ></ScriptEditor>
      ) : (
        <></>
      )}
    </div>
  );
};

export default ShaderSettings;
