# EffectSong
[effectsong.com](https://effectsong.com)

EffectSong is an AI-powered visualizer that generates shaders from text, creating dynamic, music-synchronized graphics.

## Development Components

### Renderer (C++ / OpenGL → WASM)
- Renders real-time visual effects based on music attributes and AI-generated GLSL shaders.
- Supports cubemap IBL, skybox, and glTF scene rendering.

### Frontend (React / WASM Integration)
- Provides an intuitive UI with editing tools.
- Integrates with the WASM-based renderer.

### Backend (Node.js / API Server)
- Handles AI processing and API requests.

### AI (currently using Claude API)
- Converts user text descriptions into GLSL shader effects.
- Generates shader code based on audio input and predefined GLSL helper functions.

## Principles

### Simple Interface
Users describe effects in text, and AI automatically generates GLSL code.

### Immediate Feedback
Visual effects respond to music in real time as users upload their tracks.

## Development Plan

### **2024 Q4: PoC**
- Implement the renderer (core features, WASM compilation).
- Develop frontend:
  - Music player
  - Import support for MP3/WAV/FLAC/GLTF
  - Controls: Hierarchy, TRS, Light, Camera
  - Shader scripting tools

### **2025 Q1: Prototype**
- **Rendering Engine**: Complete PBR implementation.
- **Frontend**: Migrate to TypeScript, Vite.
- **Backend**: Develop AI API using claude.
- **AI**: Enable AI-driven GLSL generation from user text input.

### **2025 Q2: Alpha**
- **Rendering Engine**: Implement glTF animation and skinning; migrate to WebGPU.
- **Frontend**: Implement a basic community feature.
- **Backend**: Implement a basic community feature.
- **AI**: Begin development of a GLSL (or WGSL) generation model.

### **2025 Q3: Beta**
- **Rendering Engine**: Polish and optimize rendering features. test rendering on Android via JNI.
- **Frontend**: improve UI/UX and interactivity.
- **Backend**: improve backend logic and scalability.
- **AI**: Finalize the GLSL (or WGSL) generation model and run on-device inference in the browser via WebAssembly and WebGPU for optimal performance.
