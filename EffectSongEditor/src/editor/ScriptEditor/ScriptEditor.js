import { useEffect } from 'react';
import Editor, { loader } from "@monaco-editor/react";
import { Box} from "@mui/material";
import ShaderSettings from './ShaderSettings';

function ScriptEditor({ module, vertexShader, setVertexShader}) {
    useEffect(() => {
        loader.init().then((monaco) => {
            // 언어 등록
            monaco.languages.register({ id: "glsles" });

            // Monarch 구문 정의
            monaco.languages.setMonarchTokensProvider("glsles", {
                keywords: [
                    // Storage Qualifiers
                    "in", "out", "uniform", "buffer", "shared", "precision", "attribute",
                    // Data Types
                    "void", "bool", "int", "uint", "float",
                    "vec2", "vec3", "vec4", "mat2", "mat3", "mat4",
                    "sampler2D", "samplerCube", "sampler2DArray", "sampler3D",
                    // Control Statements
                    "if", "else", "while", "do", "for", "break", "continue", "return",
                    "discard", "switch", "case", "default",
                ],
                operators: [
                    "=", "+", "-", "*", "/", "%", "==", "!=", "<", ">", "<=", ">=",
                    "&&", "||", "!", "++", "--"
                ],
                builtins: [
                    // Built-in Functions
                    "abs", "sin", "cos", "tan", "asin", "acos", "atan",
                    "pow", "exp", "log", "sqrt", "inversesqrt",
                    "min", "max", "clamp", "mix", "step", "smoothstep",
                    "length", "distance", "dot", "cross", "normalize",
                    "reflect", "refract", "matrixCompMult",
                    "texture", "textureProj", "textureLod", "textureGrad",
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
                                    "@default": "identifier"
                                }
                            }
                        ],
                        // Numbers
                        [/\d*\.\d+([eE][\-+]?\d+)?/, "number.float"],
                        [/\d+/, "number"],
                        // Operators and symbols
                        [/@symbols/, { cases: { "@operators": "operator", "@default": "" } }],
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
                top: "0px",
                left: "300px",
                width: "calc(100% -  650px)",
                height: "calc(100vh - 60px)",
                backgroundColor: "rgba(0.0, 0, 0, 0.2)",
                boxShadow: 3,
                pointerEvents: "auto"
            }}
        >
            <ShaderSettings module={module}/>
            <Editor
                height="calc(100% - 50px)"
                defaultLanguage="glsles"
                defaultValue={vertexShader}
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