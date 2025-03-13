import Anthropic from "@anthropic-ai/sdk";
import express from "express";
import cors from 'cors';

const anthropic = new Anthropic();

const app = express();
app.use(cors());

const port = 7777;
app.get("/stream", async (req, res) => {
  console.log(req);

  let userRequest = `You are a professional Shader Artist and a creative technical expert highly proficient in OpenGL ES 3.0.

Please implement the visual goals with the following characteristics at the highest level of visual effects:
•	Motion: Smooth and natural dynamic expression with dazzling animations
•	Color: Elegant and harmonious color composition with rich gradients
•	Responsiveness: Dynamic and organic changes based on music and time
•	Sense of Space: Overwhelming depth with multi-layered effects
•	Texture: Rich surface details and organic texture representation
•	Consistency: Smooth transitions between frames and stable motion
•	Precision: Delicate details and fine-grained variations

### **Development Environment - OpenGL ES 3.0-based Vertex Shader**
**Cautions:**
•	Vector/matrix operations must have exact type matching.
•	Swizzling results cannot be used directly in calculations (store in a temporary variable first).
•	All float values must include a decimal point (e.g., 1.0).
•	Arrays can only be accessed with constant indices.
•	const arrays are not supported.

### **Header Information:**
•	layout (location = 0) in float vertexId: Index of the current vertex.
•	uniform vec4 background: Background color (RGBA, values in the range 0.0–1.0).
•	uniform float time: Music playback time (in seconds).
•	uniform float vertexCount: Total number of vertices.
•	uniform sampler2D sound, uniform sampler2D sound2: Sound texture data.  
  - Texture size: (2048, 2048), Y-axis = time, X-axis = FFT per frequency.
  - New data is added each frame while old data scrolls upward.
•	uniform bool isStereo: Whether stereo sound is used.
•	out vec4 v_color: Output color to the fragment shader.
•	gl_Position: Must output **NDC coordinates**.
•	gl_PointSize: Required in Points Primitive Mode.

---

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

---

### **Spatial Utilization Guidelines**
•	**NDC Coordinate System:**  
  - **Ensure gl_Position is in the normalized device coordinate (NDC) range (-1 to 1).**  
  - **Do not return view-space coordinates as the final output.**  
  - Prevent coordinate overflow.  
•	**3D Space Representation:**  
  - Apply depth (z) effectively.  
  - Utilize perspective projection **internally**, but ensure the final result is NDC.  
  - Maintain correct aspect ratio to prevent distortion.  
•	**3D Scene Construction:**  
  - Optimize camera movement and field of view.  
  - Use **overlapping layers** for depth effects.  
  - **Ensure perspective projection is applied before NDC transformation.**  

---

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

---

### **Optimization Requirements**
•	**Efficient mathematical function usage** (e.g., avoid redundant calculations).  
•	**Smooth interpolation and animation transitions.**  
•	**Optimize vertex reuse and index calculations** for efficiency.  
•	**Apply proper depth sorting and back-face culling.**  

---

### **Implementation Style**
•	**Clear comments** for all major functions.  
•	**Modular and reusable** function structure.  
•	**Optimized vertex indexing** to ensure smooth rendering.  

---

### **Final Request**
Based on your request **${req.query.request}**, please **creatively interpret** the above structure to generate an optimized **${req.query.primitiveMode}** primitive mode Vertex Shader.

Assume the header information is already implemented, so exclude it from the response.

The response should contain only the raw GLSL code with no additional comments, explanations, or formatting.
        `;

  res.setHeader('Content-Type', 'text/event-stream');
  res.setHeader('Cache-Control', 'no-cache');
  res.setHeader('Connection', 'keep-alive');

  await anthropic.messages
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
    .on("end", ()=>{
      console.log('정상 종료');
      res.end();
    })
    .on("error", (error)=>{
      console.log('[ERROR] 비정상 종료: ',error);
      res.write(`data: <ES_AI_ERROR>: ${error.name}`);
      res.end();
    });

    req.on("close", ()=>{
      res.end();
    });
});

const server = app.listen(port, ()=>{
    console.log('server on ', port);
})
