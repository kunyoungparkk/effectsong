import { useState, useEffect } from "react";
import Editor, { loader } from "@monaco-editor/react";
import { Box, Button, Grid, Typography, TextField } from "@mui/material";
import PlayArrowIcon from '@mui/icons-material/PlayArrow';
import AutoAwesomeIcon from '@mui/icons-material/AutoAwesome';

function ScriptEditor({
  module,
  vertexShader,
  setVertexShader,
  opacity,
  setPrimitiveMode,
  setVertexCount,
  targetShaderIndex,
  setTargetShaderIndex,
  compileShader
}: any) {
  const [aiRequest, setAIRequest] = useState("");

  useEffect(() => {
    loader.init().then((monaco) => {
      // 언어 등록
      monaco.languages.register({ id: "glsles" });

      // Monarch 구문 정의
      monaco.languages.setMonarchTokensProvider("glsles", {
        keywords: [
          // Storage Qualifiers
          "in",
          "out",
          "uniform",
          "buffer",
          "shared",
          "precision",
          "attribute",
          // Data Types
          "void",
          "bool",
          "int",
          "uint",
          "float",
          "vec2",
          "vec3",
          "vec4",
          "mat2",
          "mat3",
          "mat4",
          "sampler2D",
          "samplerCube",
          "sampler2DArray",
          "sampler3D",
          // Control Statements
          "if",
          "else",
          "while",
          "do",
          "for",
          "break",
          "continue",
          "return",
          "discard",
          "switch",
          "case",
          "default",
        ],
        operators: [
          "=",
          "+",
          "-",
          "*",
          "/",
          "%",
          "==",
          "!=",
          "<",
          ">",
          "<=",
          ">=",
          "&&",
          "||",
          "!",
          "++",
          "--",
        ],
        builtins: [
          // Built-in Functions
          "abs",
          "sin",
          "cos",
          "tan",
          "asin",
          "acos",
          "atan",
          "pow",
          "exp",
          "log",
          "sqrt",
          "inversesqrt",
          "min",
          "max",
          "clamp",
          "mix",
          "step",
          "smoothstep",
          "length",
          "distance",
          "dot",
          "cross",
          "normalize",
          "reflect",
          "refract",
          "matrixCompMult",
          "texture",
          "textureProj",
          "textureLod",
          "textureGrad",
        ],
        symbols: /[=><!~?:&|+\-*\/\^%]+/,
        tokenizer: {
          root: [
            // Keywords
            [
              /\b(?:[a-zA-Z_]\w*)\b/,
              {
                cases: {
                  "@keywords": "keyword",
                  "@builtins": "builtin",
                  "@default": "identifier",
                },
              },
            ],
            // Numbers
            [/\d*\.\d+([eE][\-+]?\d+)?/, "number.float"],
            [/\d+/, "number"],
            // Operators and symbols
            [
              /@symbols/,
              { cases: { "@operators": "operator", "@default": "" } },
            ],
            // Comments
            [/\/\/.*$/, "comment"],
            [/\/\*.*?\*\//, "comment"],
          ],
        },
      });

      // 언어 설정
      monaco.languages.setLanguageConfiguration("glsles", {
        comments: {
          lineComment: "//",
          blockComment: ["/*", "*/"],
        },
        brackets: [
          ["{", "}"],
          ["[", "]"],
          ["(", ")"],
        ],
        autoClosingPairs: [
          { open: "{", close: "}" },
          { open: "[", close: "]" },
          { open: "(", close: ")" },
          { open: '"', close: '"' },
          { open: "'", close: "'" },
        ],
      });

      monaco.editor.defineTheme("transparentTheme", {
        base: "vs-dark",
        inherit: true,
        rules: [],
        colors: {
          "editor.background": "#00000000", // 완전 투명
        },
      });
    });
  }, []);

  // 에디터 렌더링
  return (
    <Box
      sx={{
        position: "fixed",
        top: "40px",
        left: "300px",
        width: "calc(100% -  650px)",
        height: "calc(100vh - 130px)",
        backgroundColor: `rgba(0.0, 0.0, 0.0, ${opacity})`,
        pointerEvents: "auto",
      }}
    >
      <Box
        sx={{
          width: "100%",
          height: "50px",
          backgroundColor: `rgba(255.0, 255.0, 255.0, ${opacity})`,
          pointerEvents: "auto",
        }}
      >
        <Grid
          container
          columnSpacing={{ xs: 1, sm: 2, md: 3 }}
          sx={{
            paddingTop: 1.2,
            paddingLeft: 2,
            paddingRight: 2,
            paddingBottom: 1,
            width: "100%",
            height: "100%",
          }}
        >
          <Grid item xs={8}>
            <TextField id="standard-basic" variant="standard"
              value={aiRequest}
              onChange={(e) => { setAIRequest(e.target.value); }}
              sx={{ width: '100%', height: '50%' }} />

          </Grid>
          <Grid item xs={1}>
            <Button
              size="small"
              variant="contained"
              startIcon={< AutoAwesomeIcon />}
              sx={
                { width: "100%", color: "white" }
              }
              onClick={() => {
                //send request
              }}
            >
            </Button>
          </Grid>
          <Grid item xs={1}></Grid>
          <Grid item xs={2}>
            <Button
              size="small"
              variant="contained"
              startIcon={< PlayArrowIcon />}
              sx={
                { width: "100%", color: "white" }
              }
              onClick={() => {
                compileShader(vertexShader);
              }}
            >
              <Typography
                variant="body2"
                noWrap
                style={{
                  overflow: "hidden",
                  textOverflow: "ellipsis",
                  whiteSpace: "nowrap",
                }}
              >
                compile
              </Typography>
            </Button>
          </Grid>

        </Grid>
      </Box>

      <Box
        sx={{
          width: "100%",
          height: "50px",
          pointerEvents: "auto",
        }}
      >

      </Box>
      <Editor
        height="calc(100% - 100px)"
        defaultLanguage="glsles"
        value={vertexShader}
        theme="transparentTheme"
        onChange={(value) => {
          setVertexShader(value);
        }}
        options={{
          fontSize: 14,
          minimap: { enabled: false },
          readOnly: false,
        }}
      />
    </Box>
  );
}

export default ScriptEditor;

{/* <Grid item xs={2}>
            <Button
              size="small"
              variant="contained"
              startIcon={<CodeIcon />}
              sx={
                true //그냥 탭선택으로 해야
                  ? { width: "100%", color: "black" }
                  : { width: "100%" }
              }
              onClick={() => {
                setTargetShaderIndex(0);
              }}
            >
              <Typography
                variant="body2"
                noWrap
                style={{
                  overflow: "hidden",
                  textOverflow: "ellipsis",
                  whiteSpace: "nowrap",
                }}
              >
                default
              </Typography>
            </Button>
          </Grid> */}

{/* <Grid item xs={2}>
            <Button
              size="small"
              variant="contained"
              startIcon={<CodeIcon />}
              sx={
                presetIndex === 1
                  ? { width: "100%", color: "black" }
                  : { width: "100%" }
              }
              onClick={() => {
                setVertexShader(stereoVS);
                module.ArtShader.getInstance().setPrimitiveMode(0);
                setPrimitiveMode(0);
                module.ArtShader.getInstance().setVertexCount(10000);
                setVertexCount(10000);
                setPresetIndex(1);
              }}
            >
              <Typography
                variant="body2"
                noWrap
                style={{
                  overflow: "hidden",
                  textOverflow: "ellipsis",
                  whiteSpace: "nowrap",
                }}
              >
                stereo circles
              </Typography>
            </Button>
          </Grid>
          <Grid item xs={2}>
            <Button
              size="small"
              variant="contained"
              startIcon={<CodeIcon />}
              sx={
                presetIndex === 2
                  ? { width: "100%", color: "black" }
                  : { width: "100%" }
              }
              onClick={() => {
                setVertexShader(soundTextureVS);
                module.ArtShader.getInstance().setPrimitiveMode(0);
                setPrimitiveMode(0);
                module.ArtShader.getInstance().setVertexCount(50000);
                setVertexCount(50000);
                setPresetIndex(2);
              }}
            >
              <Typography
                variant="body2"
                noWrap
                style={{
                  overflow: "hidden",
                  textOverflow: "ellipsis",
                  whiteSpace: "nowrap",
                }}
              >
                sound texture
              </Typography>
            </Button>
          </Grid>
          <Grid item xs={2}>
            <Button
              size="small"
              variant="contained"
              startIcon={<CodeIcon />}
              sx={
                presetIndex === 3
                  ? { width: "100%", color: "black" }
                  : { width: "100%" }
              }
              onClick={() => {
                setVertexShader(spinning3DVS);
                module.ArtShader.getInstance().setPrimitiveMode(4);
                setPrimitiveMode(4);
                module.ArtShader.getInstance().setVertexCount(50000);
                setVertexCount(50000);
                setPresetIndex(3);
              }}
            >
              <Typography
                variant="body2"
                noWrap
                style={{
                  overflow: "hidden",
                  textOverflow: "ellipsis",
                  whiteSpace: "nowrap",
                }}
              >
                3D spinning
              </Typography>
            </Button>
          </Grid>
          <Grid item xs={2}>
            <Button
              size="small"
              variant="contained"
              startIcon={<CodeIcon />}
              sx={
                presetIndex === 4
                  ? { width: "100%", color: "black" }
                  : { width: "100%" }
              }
              onClick={() => {
                setVertexShader(nightStarsVS);
                module.ArtShader.getInstance().setPrimitiveMode(4);
                setPrimitiveMode(4);
                module.ArtShader.getInstance().setVertexCount(50000);
                setVertexCount(50000);
                setPresetIndex(4);
              }}
            >
              <Typography
                variant="body2"
                noWrap
                style={{
                  overflow: "hidden",
                  textOverflow: "ellipsis",
                  whiteSpace: "nowrap",
                }}
              >
                night stars
              </Typography>
            </Button>
          </Grid>
          <Grid item xs={2}>
            <Button
              size="small"
              variant="contained"
              startIcon={<CodeIcon />}
              sx={
                presetIndex === 5
                  ? { width: "100%", color: "black" }
                  : { width: "100%" }
              }
              onClick={() => {
                setVertexShader(grassVS);
                module.ArtShader.getInstance().setPrimitiveMode(4);
                setPrimitiveMode(4);
                module.ArtShader.getInstance().setVertexCount(50000);
                setVertexCount(50000);
                setPresetIndex(5);
              }}
            >
              <Typography
                variant="body2"
                noWrap
                style={{
                  overflow: "hidden",
                  textOverflow: "ellipsis",
                  whiteSpace: "nowrap",
                }}
              >
                grass
              </Typography>
            </Button>
          </Grid> */}