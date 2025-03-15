"use strict";
var __awaiter = (this && this.__awaiter) || function (thisArg, _arguments, P, generator) {
    function adopt(value) { return value instanceof P ? value : new P(function (resolve) { resolve(value); }); }
    return new (P || (P = Promise))(function (resolve, reject) {
        function fulfilled(value) { try { step(generator.next(value)); } catch (e) { reject(e); } }
        function rejected(value) { try { step(generator["throw"](value)); } catch (e) { reject(e); } }
        function step(result) { result.done ? resolve(result.value) : adopt(result.value).then(fulfilled, rejected); }
        step((generator = generator.apply(thisArg, _arguments || [])).next());
    });
};
var __importDefault = (this && this.__importDefault) || function (mod) {
    return (mod && mod.__esModule) ? mod : { "default": mod };
};
Object.defineProperty(exports, "__esModule", { value: true });
const sdk_1 = __importDefault(require("@anthropic-ai/sdk"));
const express_1 = __importDefault(require("express"));
const cors_1 = __importDefault(require("cors"));
const https_1 = __importDefault(require("https"));
const fs_1 = __importDefault(require("fs"));
const port = 7443;
const anthropic = new sdk_1.default();
const app = (0, express_1.default)();
app.use((0, cors_1.default)());
const options = {
    key: fs_1.default.readFileSync('/etc/letsencrypt/live/api.effectsong.com/privkey.pem'),
    cert: fs_1.default.readFileSync('/etc/letsencrypt/live/api.effectsong.com/fullchain.pem'),
};
app.get("/stream", (req, res) => __awaiter(void 0, void 0, void 0, function* () {
    console.log('요청 수신');
    let userRequest = `You are a professional Shader Artist and a creative technical expert highly proficient in OpenGL ES 3.0.

Please implement the visual goals with the following characteristics at the highest level of visual effects:
•	Motion: Smooth and natural dynamic expression with dazzling animations
•	Color: Elegant and harmonious color composition with rich gradients
•	Responsiveness: Dynamic and organic changes based on music and time
•	Consistency: Smooth transitions between frames and stable motion
•	Precision: Delicate details and fine-grained variations

### **Development Environment - OpenGL ES 3.0-based Vertex Shader**
**Cautions:**
•	Vector/matrix operations must have exact type matching.
•	Swizzling results cannot be used directly in calculations (store in a temporary variable first).
•	All float values must include a decimal point (e.g., 1.0).
•	Arrays can only be accessed with constant indices.
•	const arrays are not supported.
•	Always ensure alpha values are not unintentionally reduced below a visible threshold.
•	Ensure the final vertex positions (gl_Position) are clearly represented within the NDC coordinate space, especially the Z-axis, which must strictly remain between -1.0 and 1.0.

### **Header Information:**
•	layout (location = 0) in float vertexId: Index of the current vertex.
•	uniform float time: Music playback time (in seconds).
•	uniform float vertexCount: Total number of vertices.
•	uniform sampler2D sound, uniform sampler2D sound2: Sound texture data.  
  - Texture size: (2048, 2048), Y-axis = time, X-axis = FFT results by frequency
  - Y=0 represents the most recent data
  - New data is added each frame while existing data scrolls upward one line at a time
  - By utilizing this structure, you can access the sound's history, allowing you to create effects like waves gradually spreading outward from the center - this approach is recommended
  - For stereo audio: sound2 contains the right channel data; for mono audio: sound and sound2 have identical values
•	uniform bool isStereo: Whether stereo sound is used.
•	out vec4 v_color: Output color to the fragment shader.
•	gl_Position: Must output **NDC coordinates**.
•	gl_PointSize: Required in Points Primitive Mode.

### **Primitive Mode**
•	**Points:**  
  - Suitable for particle effects or constellations.
  - Utilize gl_PointSize for dynamic size changes.  
•	**Lines:**  
  - Ideal for continuous patterns or trajectories.  
  - Connects each vertex sequentially.  
•	**Triangles (Realistic 3D Mesh Optimization):**  
  - Create continuous 3D surfaces by ensuring vertex connectivity.
  - Utilize spherical projection, polar coordinates, sinusoidal deformations, and fractal recursion for advanced shape generation.
  - Optimize vertex processing by efficiently reusing vertex buffers and sharing vertices between adjacent triangles.
  - Use indexed drawing and strip structures to enhance rendering performance.
  - Compute vertex indices efficiently to ensure structured quad-based triangulation:
  cols = floor(sqrt(vertexCount / 6.0)) // A quad consists of two triangles.
  rows = cols
  quadId = floor(vertexId / 6.0)
  vertexInQuad = mod(vertexId, 6.0)
  quadRow = floor(quadId / (cols - 1))
  quadCol = mod(quadId, cols - 1) 
  - Ensure the final output of gl_Position is in NDC space by normalizing quad positions:
  position = vec2((quadCol + offset.x) / (cols - 1.0), (quadRow + offset.y) / (rows - 1.0)) * 2.0 - 1.0;
  - Guarantee smooth, connected surfaces by preventing gaps between triangles.

### **Spatial Utilization Guidelines**
•	**NDC Coordinate System:**  
  - **Ensure gl_Position is in the normalized device coordinate (NDC) range (-1 to 1).**  
  - **return ndc-space coordinates as the final output.**  
  - Prevent coordinate overflow.  
•	**3D Space Representation:**  
  - Maintain correct aspect ratio to prevent distortion.  
•	**3D Scene Construction:**  
  - Optimize camera movement and field of view.  
  - Use **overlapping layers** for depth effects.  
  - **Ensure perspective projection is applied before NDC transformation.**  

### **Code Structure Guidelines**
1. **Utility Functions:**  
   - 2D rotation matrix  
   - Random/noise functions  
   - Smooth interpolation  
2. **Position Calculation:**  
   - **Ensure seamless vertex connectivity.**  
   - **Compute normal vectors dynamically for shading.**  
   - **Transform vertices using camera projection but output NDC coordinates.**  
   - Time-based animations for motion.  
3. **Color & Visual Effects:**  
   - Gradients and brightness variation.  
   - Time-dependent transitions.  
4. **Music Responsiveness:**  
   - Dynamic effects based on sound texture.  
   - Handle stereo/mono sound variations. 
   - Use FFT-based frequency bands from sound and sound2 to influence animation over time, ensuring smooth transitions and avoiding abrupt changes.
   5. **GLSL ES 3.0 Compliance:**  
   - Use precise float literals (e.g., 1.0, 0.5).  
   - Ensure correct type casting and operations. 

### **Implementation Style**
•	**Clear comments** for all major functions.  
•	**Modular and reusable** function structure.  
•	**Optimized vertex indexing** to ensure smooth rendering.  

### **Examples of music responsive visual patterns**
- **Spiral wave:**:
// Extracting and processing music data
float extractSound(float frequency, float timeOffset) {
return texture(sound, vec2(frequency, timeOffset/2048.0)).r;
}
// Implementing spiral patterns
float angle = vertexId / vertexCount * 3.14159 * 2.0;
float spread = 0.1; // frequency interval
float off = 0.1; // start frequency
float speed = 0.0; // current time frame
// Smooth response with multiple frequency averages
float snd = (
extractSound(off, speed) +
extractSound(off + spread, speed) +
extractSound(off + spread * 2.0, speed)
) / 3.0;
float pulse = power(snd, 5.0); // strong reactivity
float radius = vertexId * 0.02 + pulse * 0.4; // 반경 변화
// Location calculation
vec2 xy = vec2(
cos(angle + time) * radius,
sin(angle + time) * radius
);
// Application of screen ratio and output of NDC coordinates
gl_Position = vec4(xy, 0.0, 1.0);

### **Final Request**
Based on your request **${req.query.request}**, please **creatively interpret** the above structure to generate an optimized **${req.query.primitiveMode}** primitive mode Vertex Shader.

Assume the header information is already implemented, so exclude it from the response.

The response should contain only the raw GLSL code with no additional comments, explanations, or formatting.
        `;
    res.setHeader('Content-Type', 'text/event-stream');
    res.setHeader('Cache-Control', 'no-cache');
    res.setHeader('Connection', 'keep-alive');
    yield anthropic.messages
        .stream({
        model: "claude-3-7-sonnet-20250219",
        max_tokens: 8192,
        temperature: 0,
        messages: [
            {
                role: "user",
                content: [
                    {
                        type: "text",
                        text: userRequest
                    },
                ],
            },
        ],
    })
        .on("text", (text) => {
        res.write(`data: ${text.replace(/\n/g, "\\n")}\n\n`);
    })
        .on("end", () => {
        console.log('정상 종료');
        res.end();
    })
        .on("error", (error) => {
        console.log('[ERROR] 비정상 종료: ', error);
        res.write(`data: <ES_AI_ERROR>: ${error.name}`);
        res.end();
    });
    req.on("close", () => {
        res.end();
    });
}));
const server = https_1.default.createServer(options, app).listen(port, () => {
    console.log('server on ', port);
});
