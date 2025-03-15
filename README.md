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

### AI (Claude API)
- Converts user text descriptions into GLSL shader effects.
- Generates shader code based on audio input and predefined GLSL helper functions.

## Principles

### Simple Interface
Users describe effects in text, and AI automatically generates GLSL code.

### Immediate Feedback
Visual effects respond to music in real time as users upload their tracks.

## Development Plan

### **2024: Prototype**
- Implement the renderer (core features, WASM compilation).
- Develop frontend:
  - Music player
  - Import support for MP3/WAV/FLAC/GLTF
  - Controls: Hierarchy, TRS, Light, Camera
  - Shader scripting tools

### **2025 Q1: Alpha Version**
- **Rendering Engine**: Complete PBR implementation.
- **Frontend**: Migrate to TypeScript, Vite.
- **Backend**: Develop AI API.
- **AI**: Enable AI-driven GLSL generation from user text input.

### **2025 Q2: Beta Version**
- Further refinements and optimizations.
