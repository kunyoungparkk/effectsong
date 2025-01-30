# EffectSong.
effectsong.com

3D audio visualizer & editor

Goal: Popularizing audio-interactive shader art and 3D content creation through AI technology

# Development Components
#### Renderer: Built with C++ / OpenGL, compiled to WASM
1. Renders visual effects based on specific attributes of the input music file and the GLSL file. (render to cubemap ibl, skybox)
2. Renders gltf scene

#### Front-End: Developed with React, integrating and interacting with WASM
Includes visual nodes, editing tools, other UI elements, and community features.

#### Back-End
Handles community functions, APIs for AI.

#### AI: Built using the Claude API
Implements an AI that generates shader effect code corresponding to user text commands based on audio input data and GLSL helper functions.

# Principles
#### Simple Interface
AI automatically generates GLSL code based on user input describing desired styles, colors, or effects in text.
Provides a node-based editor allowing users to create outcomes by combining basic visual nodes, even without writing complex shader code.

#### Immediate Feedback
Visual effects react to music in real-time as users upload their music.

#### Pre-Made Templates
Offers various visualization templates to help beginners get started easily.

# Plan

#### 2024
Prototype
- Implement renderer (including essential features) and compile to WASM
- Develop frontend: Music Player, import support for MP3/WAV/FLAC/GLTF, controls for Hierarchy/TRS/Light/Camera, Shader Script, and related settings
 
#### 2025 1Q
Alphatype
- Complete implementation of all renderer features
- Develop frontend: Visual Script, Simple Community
- Develop backend: Community, AI API
- Develop AI: Generate GLSL code based on user's text input
  
