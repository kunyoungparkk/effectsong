import { useEffect, useRef } from 'react';
import Editor, { loader } from '@monaco-editor/react';
import { Box } from '@mui/material';
import { editor } from 'monaco-editor';

type ScriptEditorType = {
  vertexShader: string;
  setVertexShader: React.Dispatch<React.SetStateAction<string>>;
  opacity: number;
  receivingCode: boolean
};
function ScriptEditor({
  vertexShader,
  setVertexShader,
  opacity,
  receivingCode
  // module,
  // setPrimitiveMode,
  // setVertexCount,
  // targetShaderIndex,
  // setTargetShaderIndex,
}: ScriptEditorType) {
  const editorRef = useRef<any>(null);

  useEffect(() => {
    loader.init().then((monaco) => {
      // 언어 등록
      monaco.languages.register({ id: 'glsles' });

      // Monarch 구문 정의
      monaco.languages.setMonarchTokensProvider('glsles', {
        keywords: [
          // Storage Qualifiers
          'in',
          'out',
          'uniform',
          'buffer',
          'shared',
          'precision',
          'attribute',
          // Data Types
          'void',
          'bool',
          'int',
          'uint',
          'float',
          'vec2',
          'vec3',
          'vec4',
          'mat2',
          'mat3',
          'mat4',
          'sampler2D',
          'samplerCube',
          'sampler2DArray',
          'sampler3D',
          // Control Statements
          'if',
          'else',
          'while',
          'do',
          'for',
          'break',
          'continue',
          'return',
          'discard',
          'switch',
          'case',
          'default',
        ],
        operators: ['=', '+', '-', '*', '/', '%', '==', '!=', '<', '>', '<=', '>=', '&&', '||', '!', '++', '--'],
        builtins: [
          // Built-in Functions
          'abs',
          'sin',
          'cos',
          'tan',
          'asin',
          'acos',
          'atan',
          'pow',
          'exp',
          'log',
          'sqrt',
          'inversesqrt',
          'min',
          'max',
          'clamp',
          'mix',
          'step',
          'smoothstep',
          'length',
          'distance',
          'dot',
          'cross',
          'normalize',
          'reflect',
          'refract',
          'matrixCompMult',
          'texture',
          'textureProj',
          'textureLod',
          'textureGrad',
        ],
        symbols: /[=><!~?:&|+\-*\/\^%]+/,
        tokenizer: {
          root: [
            // Keywords
            [
              /\b(?:[a-zA-Z_]\w*)\b/,
              {
                cases: {
                  '@keywords': 'keyword',
                  '@builtins': 'builtin',
                  '@default': 'identifier',
                },
              },
            ],
            // Numbers
            [/\d*\.\d+([eE][\-+]?\d+)?/, 'number.float'],
            [/\d+/, 'number'],
            // Operators and symbols
            [/@symbols/, { cases: { '@operators': 'operator', '@default': '' } }],
            // Comments
            [/\/\/.*$/, 'comment'],
            [/\/\*.*?\*\//, 'comment'],
          ],
        },
      });

      // 언어 설정
      monaco.languages.setLanguageConfiguration('glsles', {
        comments: {
          lineComment: '//',
          blockComment: ['/*', '*/'],
        },
        brackets: [
          ['{', '}'],
          ['[', ']'],
          ['(', ')'],
        ],
        autoClosingPairs: [
          { open: '{', close: '}' },
          { open: '[', close: ']' },
          { open: '(', close: ')' },
          { open: '"', close: '"' },
          { open: "'", close: "'" },
        ],
      });
      monaco.editor.defineTheme('transparentTheme', {
        base: 'vs-dark',
        inherit: true,
        rules: [],
        colors: {
          'editor.background': '#00000000', // 완전 투명
        },
      });
    });
  }, []);
  useEffect(()=>{
    if(receivingCode){
      if(editorRef.current){
        const model = editorRef.current.getModel();
        if(model){
          const lastLine = model.getLineCount();
          editorRef.current.setPosition({
            lineNumber: lastLine,
            column: model.getLineMaxColumn(lastLine)
          });
          editorRef.current.revealLine(lastLine);
        }
      }
    }
  }, [vertexShader])
  // 에디터 렌더링
  return (
    <Box
      sx={{
        position: 'fixed',
        top: '80px',
        left: '300px',
        width: 'calc(100% -  650px)',
        height: 'calc(100vh - 170px)',
        backgroundColor: `rgba(0.0, 0.0, 0.0, ${opacity})`,
        pointerEvents: 'auto',
      }}>
      {/* <Box
        sx={{
          width: '100%',
          height: '50px',
          pointerEvents: 'auto',
        }}></Box> */}
      <Editor
        height="100%"
        defaultLanguage="glsles"
        value={vertexShader}
        theme="transparentTheme"
        onChange={(value) => {
          if (value === undefined) {
            return;
          }
          setVertexShader(value);
        }}
        onMount={(editor: editor.IStandaloneCodeEditor)=>{
          editorRef.current = editor;
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
