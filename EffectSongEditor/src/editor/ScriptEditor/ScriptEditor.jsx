import { useState, useEffect } from "react";
import Editor, { loader } from "@monaco-editor/react";
import { Box, Button, Grid, Typography, TextField } from "@mui/material";
import CodeIcon from "@mui/icons-material/Code";
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
}) {
  const simpleVS = `//shader art sample
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
  const stereoVS = `//shader art sample
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
  const spinning3DVS = `#define PI radians(180.)
    vec3 hsv2rgb(vec3 c) {
        c = vec3(c.x, clamp(c.yz, 0.0, 1.0));
        vec4 K = vec4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);
        vec3 p = abs(fract(c.xxx + K.xyz) * 6.0 - K.www);
        return c.z * mix(K.xxx, clamp(p - K.xxx, 0.0, 1.0), c.y);
    }
    
    mat4 rotY( float angle ) {
        float s = sin( angle );
        float c = cos( angle );
            
        return mat4( 
            c, 0,-s, 0,
            0, 1, 0, 0,
            s, 0, c, 0,
            0, 0, 0, 1);  
    }
    
    
    mat4 rotZ( float angle ) {
        float s = sin( angle );
        float c = cos( angle );
            
        return mat4( 
            c,-s, 0, 0, 
            s, c, 0, 0,
            0, 0, 1, 0,
            0, 0, 0, 1); 
    }
    
    mat4 trans(vec3 trans) {
        return mat4(
        1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, 1, 0,
        trans, 1);
    }
    
    mat4 ident() {
        return mat4(
        1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, 1, 0,
        0, 0, 0, 1);
    }
    
    mat4 scale(vec3 s) {
        return mat4(
        s[0], 0, 0, 0,
        0, s[1], 0, 0,
        0, 0, s[2], 0,
        0, 0, 0, 1);
    }
    
    mat4 uniformScale(float s) {
        return mat4(
        s, 0, 0, 0,
        0, s, 0, 0,
        0, 0, s, 0,
        0, 0, 0, 1);
    }
    
    mat4 persp(float fov, float aspect, float zNear, float zFar) {
        float f = tan(PI * 0.5 - 0.5 * fov);
        float rangeInv = 1.0 / (zNear - zFar);
    
        return mat4(
        f / aspect, 0, 0, 0,
        0, f, 0, 0,
        0, 0, (zNear + zFar) * rangeInv, -1,
        0, 0, zNear * zFar * rangeInv * 2., 0);
    }
    
    // hash function from https://www.shadertoy.com/view/4djSRW
    float hash(float p) {
        vec2 p2 = fract(vec2(p * 5.3983, p * 5.4427));
        p2 += dot(p2.yx, p2.xy + vec2(21.5351, 14.3137));
        return fract(p2.x * p2.y * 95.4337);
    }
    
    float m1p1(float v) {
        return v * 2. - 1.;
    }
    
    float p1m1(float v) {
        return v * 0.5 + 0.5;
    }
    
    float inv(float v) {
        return 1. - v;
    }
    
    #define NUM_EDGE_POINTS_PER_CIRCLE 48.
    #define NUM_SUBDIVISIONS_PER_CIRCLE 64.
    #define NUM_POINTS_PER_DIVISION (NUM_EDGE_POINTS_PER_CIRCLE * 6.)
    #define NUM_POINTS_PER_CIRCLE (NUM_SUBDIVISIONS_PER_CIRCLE * NUM_POINTS_PER_DIVISION) 
    void getCirclePoint(const float id, const float inner, const float start, const float end, out vec3 pos, out vec3 uvf, out float snd) {
        float edgeId = mod(id, NUM_POINTS_PER_DIVISION);
        float ux = floor(edgeId / 6.) + mod(edgeId, 2.);
        float vy = mod(floor(id / 2.) + floor(id / 3.), 2.); // change that 3. for cool fx
        float sub = floor(id / NUM_POINTS_PER_DIVISION);
        float subV = sub / (NUM_SUBDIVISIONS_PER_CIRCLE - 1.);
        float level = subV + vy / (NUM_SUBDIVISIONS_PER_CIRCLE - 1.);
        float u = ux / NUM_EDGE_POINTS_PER_CIRCLE;
        float v = 1.;//mix(inner, 1., level);
        float ringId = sub + vy;
        float ringV = ringId / NUM_SUBDIVISIONS_PER_CIRCLE;
        float a = mix(start, end, u) * PI * 2. + PI * 0.0;
        float skew = 1. - step(0.5, mod(ringId - 2., 3.));
        snd = texture(sound, vec2((0.02 + abs(u * 2. - 1.)) * 0.10, ringV * 0.25)).r;
        
        
        a += 1. / NUM_EDGE_POINTS_PER_CIRCLE * PI * 2. * 20. * sin(time * 1.) + snd * 1.5;
        float s = sin(a);
        float c = cos(a);
        float x = c * v;
        float y = s * v;
        float z = mix(inner, 1., level);
        pos = vec3(x, y, z);  
        uvf  = vec3(floor(edgeId / 6.) / NUM_EDGE_POINTS_PER_CIRCLE, subV, floor(id / 6.));
    }
    
    float goop(float t) {
        return sin(t) + sin(t * 0.27) + sin(t * 0.13) + sin(t * 0.73);
    }
    
    void main() {
        float circleId = floor(vertexId / NUM_POINTS_PER_CIRCLE);
        float pointId = mod(vertexId, NUM_POINTS_PER_CIRCLE);
    //  float sideId = floor(circleId / 2.);
    //  float side = mix(-1., 1., step(0.5, mod(circleId, 2.)));
        float numCircles = floor(vertexCount / NUM_POINTS_PER_CIRCLE);
        float cu = circleId / numCircles;
        vec3 pos;
        float inner = 0.5;
        float start = 0.;//time * -0.1;
        float end   = start + 1.;
        float snd;
        vec3 uvf;
        getCirclePoint(pointId, inner, start, end, pos, uvf, snd); 
        
        vec3 offset = vec3(0);
        //offset   += vec3(m1p1(hash(circleId)) * 0.5, m1p1(hash(circleId * 0.37)), -m1p1(circleId / numCircles));
        //offset.x += goop(circleId + time * 0.3) * 0.4;
        //offset.y += goop(circleId + time * 0.13) * 0.1;
        offset.z += 10.;
    //  vec3 aspect = vec3(1, resolution.x / resolution.y, 1);
        
        mat4 mat = ident(); 
        mat *= persp(radians(65.), resolution.x / resolution.y, 0.1, 100.);
        mat *= trans(offset);
    //  mat *= uniformScale(mix(0.1, 0.4, hash(circleId) * exp(snd)));
        mat *= scale(vec3(0.25, 0.25, -20.));
        gl_Position = mat * vec4(pos, 1);
        gl_PointSize = 4.;
    
        float hue = mix(.65, 1., step(0.75, snd));
        float sat = 1.;
        float val = 1.;
        v_color = vec4(hsv2rgb(vec3(hue, sat, val)), pow(snd + 0.1, 5.));
        v_color = mix(v_color.rgba, vec4(1,1,1,1), mod(uvf.z, 2.));
        v_color.a = 1.0 - uvf.y;
        v_color = vec4(v_color.rgb * v_color.a, v_color.a);
    }
        `;
  const grassVS = `#define PI radians(180.)
    
    vec3 hsv2rgb(vec3 c) {
      c = vec3(c.x, clamp(c.yz, 0.0, 1.0));
      vec4 K = vec4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);
      vec3 p = abs(fract(c.xxx + K.xyz) * 6.0 - K.www);
      return c.z * mix(K.xxx, clamp(p - K.xxx, 0.0, 1.0), c.y);
    }
    
    mat4 rotX(float angleInRadians) {
        float s = sin(angleInRadians);
        float c = cos(angleInRadians);
          
        return mat4( 
          1, 0, 0, 0,
          0, c, s, 0,
          0, -s, c, 0,
          0, 0, 0, 1);  
    }
    
    mat4 rotY(float angleInRadians) {
        float s = sin(angleInRadians);
        float c = cos(angleInRadians);
          
        return mat4( 
          c, 0,-s, 0,
          0, 1, 0, 0,
          s, 0, c, 0,
          0, 0, 0, 1);  
    }
    
    mat4 rotZ(float angleInRadians) {
        float s = sin(angleInRadians);
        float c = cos(angleInRadians);
          
        return mat4( 
          c,-s, 0, 0, 
          s, c, 0, 0,
          0, 0, 1, 0,
          0, 0, 0, 1); 
    }
    
    mat4 trans(vec3 trans) {
      return mat4(
        1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, 1, 0,
        trans, 1);
    }
    
    mat4 ident() {
      return mat4(
        1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, 1, 0,
        0, 0, 0, 1);
    }
    
    mat4 scale(vec3 s) {
      return mat4(
        s[0], 0, 0, 0,
        0, s[1], 0, 0,
        0, 0, s[2], 0,
        0, 0, 0, 1);
    }
    
    mat4 uniformScale(float s) {
      return mat4(
        s, 0, 0, 0,
        0, s, 0, 0,
        0, 0, s, 0,
        0, 0, 0, 1);
    }
    
    mat4 persp(float fov, float aspect, float zNear, float zFar) {
      float f = tan(PI * 0.5 - 0.5 * fov);
      float rangeInv = 1.0 / (zNear - zFar);
    
      return mat4(
        f / aspect, 0, 0, 0,
        0, f, 0, 0,
        0, 0, (zNear + zFar) * rangeInv, -1,
        0, 0, zNear * zFar * rangeInv * 2., 0);
    }
    
    mat4 trInv(mat4 m) {
      mat3 i = mat3(
        m[0][0], m[1][0], m[2][0], 
        m[0][1], m[1][1], m[2][1], 
        m[0][2], m[1][2], m[2][2]);
      vec3 t = -i * m[3].xyz;
        
      return mat4(
        i[0], t[0], 
        i[1], t[1],
        i[2], t[2],
        0, 0, 0, 1);
    }
    
    mat4 lookAt(vec3 eye, vec3 target, vec3 up) {
      vec3 zAxis = normalize(eye - target);
      vec3 xAxis = normalize(cross(up, zAxis));
      vec3 yAxis = cross(zAxis, xAxis);
    
      return mat4(
        xAxis, 0,
        yAxis, 0,
        zAxis, 0,
        eye, 1);
    }
    
    mat4 cameraLookAt(vec3 eye, vec3 target, vec3 up) {
      #if 1
      return inverse(lookAt(eye, target, up));
      #else
      vec3 zAxis = normalize(target - eye);
      vec3 xAxis = normalize(cross(up, zAxis));
      vec3 yAxis = cross(zAxis, xAxis);
    
      return mat4(
        xAxis, 0,
        yAxis, 0,
        zAxis, 0,
        -dot(xAxis, eye), -dot(yAxis, eye), -dot(zAxis, eye), 1);  
      #endif
      
    }
    
    // hash function from https://www.shadertoy.com/view/4djSRW
    float hash(float p) {
        vec2 p2 = fract(vec2(p * 5.3983, p * 5.4427));
        p2 += dot(p2.yx, p2.xy + vec2(21.5351, 14.3137));
        return fract(p2.x * p2.y * 95.4337);
    }
    
    // times 2 minus 1
    float t2m1(float v) {
      return v * 2. - 1.;
    }
    
    // times .5 plus .5
    float t5p5(float v) {
      return v * 0.5 + 0.5;
    }
    
    float inv(float v) {
      return 1. - v;
    }
    
    void getCirclePoint(const float numEdgePointsPerCircle, const float id, const float inner, const float start, const float end, out vec3 pos) {
      float outId = id - floor(id / 3.) * 2. - 1.;   // 0 1 2 3 4 5 6 7 8 .. 0 1 2, 1 2 3, 2 3 4
      float ux = floor(id / 6.) + mod(id, 2.);
      float vy = mod(floor(id / 2.) + floor(id / 3.), 2.); // change that 3. for cool fx
      float u = ux / numEdgePointsPerCircle;
      float v = mix(inner, 1., vy);
      float a = mix(start, end, u) * PI * 2. + PI * 0.0;
      float s = sin(a);
      float c = cos(a);
      float x = c * v;
      float y = s * v;
      float z = 0.;
      pos = vec3(x, y, z);  
    }
    
    
    #define CUBE_POINTS_PER_FACE 6.
    #define FACES_PER_CUBE 6.
    #define POINTS_PER_CUBE (CUBE_POINTS_PER_FACE * FACES_PER_CUBE)
    void getCubePoint(const float id, out vec3 position, out vec3 normal) {
      float quadId = floor(mod(id, POINTS_PER_CUBE) / CUBE_POINTS_PER_FACE);
      float sideId = mod(quadId, 3.);
      float flip   = mix(1., -1., step(2.5, quadId));
      // 0 1 2  1 2 3
      float facePointId = mod(id, CUBE_POINTS_PER_FACE);
      float pointId = mod(facePointId - floor(facePointId / 3.0), 6.0);
      float a = pointId * PI * 2. / 4. + PI * 0.25;
      vec3 p = vec3(cos(a), 0.707106781, sin(a)) * flip;
      vec3 n = vec3(0, 1, 0) * flip;
      float lr = mod(sideId, 2.);
      float ud = step(2., sideId);
      mat4 mat = rotX(lr * PI * 0.5);
      mat *= rotZ(ud * PI * 0.5);
      position = (mat * vec4(p, 1)).xyz;
      normal = (mat * vec4(n, 0)).xyz;
    }
    
    #if 1
    void main() {
      float id = vertexId;
      float ux = mod(id, 2.) - 0.5;
      float vy = mod(floor(id / 2.) + floor(id / 3.), 2.); // change that 3. for cool fx
      float bladeId = floor(id / 6.);
      float numBlades = floor(vertexCount / 6.);
      float down = floor(sqrt(numBlades)) * 1.;
      float across = floor(numBlades / down);
      
      float v = bladeId / numBlades;
      
      float cx = mod(bladeId, across);
      float cy = floor(bladeId / across);
      
      float cu = cx / (across - 1.);
      float cv = cy / (down - 1.);
      
      float ca = cu * 2. - 1.;
      float cd = cv * 2. - 1.;
      
      vec3 cpos = vec3(ca, 0, cd);
      vec3 pos = vec3(ux, vy, 0);
      vec3 normal = vec3(0, 0, 1);
      
      
      float snd = texture(sound, vec2(mix(0.1, 0.25, hash(v * 0.0713)), length(cpos) * .7)).r;
      
      float tm = time * 0.0125;
      float etm = tm + PI;
      mat4 mat = persp(radians(60.0), resolution.x / resolution.y, 0.1, 1000.0);
      float er = 8.;
      float tr = 8.;
      vec3 eye = vec3(cos(tm) * er, 2.2 /*sin(tm * 0.9) * 1.5*/, sin(tm) * er);
      vec3 target = vec3(cos(etm) * tr, -1., sin(etm) * tr); //vec3(-eye.x, sin(tm * 0.9) * -1.5, -eye.z) * 5.5;
      vec3 up = vec3(0, 1, 0);
      float sh = mix(0.0, 0.1, hash(v * 0.327)) + 
        mix(0.0, 0.5, sin(cu * 15.) * 0.5 + 0.5) +
        mix(0.0, 0.5, sin(cv * 15.) * 0.5 + 0.5);
      vec3 h = vec3(
        hash(v) + sin(time * 0.2 + cv * 205. + hash(cv) * 0.1) * vy * 0.2,
        0,
        hash(v * 0.123));
      normal = normalize(vec3(sin(cu * 15.), 1, cos(cv * 15.)));
      
      mat4 cmat = cameraLookAt(eye, target, up);
      mat *= cmat;
      mat *= trans(cpos * 8. + h);
      mat *= inverse(mat4(
        vec4(cmat[0].xyz, 0),
        vec4(cmat[1].xyz, 0),
        vec4(cmat[2].xyz, 0),
        vec4(0, 0, 0, 1)));
      mat *= rotZ(sin(time + v * 4.) * 0.3);
      mat *= scale(vec3(mix(0.1, 0.01, vy), 1.+ sh + pow(snd + 0.3, 10.0) * 0.1, 1));
        
      gl_Position = mat * vec4(pos, 1);
      vec3 n = normalize((mat * vec4(normal, 0)).xyz);
      
      vec3 lightDir = normalize(vec3(0.3, 0.4, -1));
      
    
      float hue = time * 0.1 + mix(0.25, 0.35, hash(bladeId * 0.0001 + vertexId * 0.0001));// * 0.25 + 0.1;//sin(time * 3.) * .1, time * 3. + .5, step(0.6, snd));//abs(ca * cd) * 2.;
      float sat = mix(0.5, 0.8, hash(bladeId * 0.0003));//step(0.6, snd);//pow(snd + 0.3, 5.);
      float val = mix(0.7, 1.0, hash(bladeId * 0.0005));
      vec3 color = hsv2rgb(vec3(hue, sat, val));
      float lit = (dot(n, lightDir) * 0.5 + 0.5);
      color = mix(color, vec3(1,1,0), pow(snd + 0.3, 10.0));
      v_color = vec4(color * mix(0., 1., lit), 1.);
      v_color.rgb *= v_color.a;
    }
    #endif
      `;
  const nightStarsVS = `float hash(float n) {
        return fract(sin(n) * 43758.5453123);
    }
    
    float noise(vec2 p) {
        vec2 i = floor(p);
        vec2 f = fract(p);
        f = f * f * (3.0 - 2.0 * f);
        return mix(
            mix(hash(i.x + i.y * 57.0), hash(i.x + 1.0 + i.y * 57.0), f.x),
            mix(hash(i.x + (i.y + 1.0) * 57.0), hash(i.x + 1.0 + (i.y + 1.0) * 57.0), f.x),
            f.y
        );
    }
    
    // 원형 방사 효과
    float radialGradient(vec2 uv, vec2 center, float radius, float intensity) {
        float dist = length(uv - center);
        return exp(-dist * intensity) * smoothstep(radius, 0.0, dist);
    }
    
    // 소용돌이 효과
    vec2 swirl(vec2 uv, vec2 center, float strength, float offset) {
        vec2 centeredUV = uv - center;
        float angle = atan(centeredUV.y, centeredUV.x) + strength * length(centeredUV) + offset;
        float radius = length(centeredUV);
        return center + vec2(cos(angle), sin(angle)) * radius;
    }
    
    // 다중 별 생성
    vec3 multipleStars(vec2 uv, vec2[15] centers, float[15] radii, float[15] intensities) {
        vec3 starColor = vec3(0.0);
        for (int i = 0; i < 15; i++) {
            float starGlow = radialGradient(uv, centers[i], radii[i], intensities[i]);
            vec3 singleStarColor = mix(
                vec3(1.0, 0.9, 0.3), // 중심부 강렬한 노란빛
                vec3(1.0, 0.7, 0.2), // 외곽부 따뜻한 주황빛
                pow(starGlow, 1.0)
            );
            starColor += singleStarColor * starGlow; // 별빛 중첩
        }
        return starColor * 1.2; // 별빛 강조
    }
    
    void main() {
        float snd = texture(sound, vec2(0.01,0.0)).r;
        // 화면 비율과 격자 크기
        float aspect = resolution.x / resolution.y;
        float gridSize = 100.0; // 격자 해상도
        float numTrianglesPerQuad = 2.0;
        float numVerticesPerQuad = numTrianglesPerQuad * 3.0;
        float quadId = floor(vertexId / numVerticesPerQuad);
        float vertexInQuad = mod(vertexId, numVerticesPerQuad);
    
        // 격자에서 사각형 위치 계산
        float quadX = mod(quadId, gridSize - 1.0);
        float quadY = floor(quadId / (gridSize - 1.0));
    
        // 사각형의 네 정점 계산
        vec2 p0 = vec2(quadX, quadY) / gridSize * 2.0 - 1.0; // 좌상단
        vec2 p1 = vec2(quadX + 1.0, quadY) / gridSize * 2.0 - 1.0; // 우상단
        vec2 p2 = vec2(quadX, quadY + 1.0) / gridSize * 2.0 - 1.0; // 좌하단
        vec2 p3 = vec2(quadX + 1.0, quadY + 1.0) / gridSize * 2.0 - 1.0; // 우하단
    
        vec2 pos;
        if (vertexInQuad < 3.0) {
            // 첫 번째 삼각형
            if (vertexInQuad == 0.0) pos = p0;
            if (vertexInQuad == 1.0) pos = p1;
            if (vertexInQuad == 2.0) pos = p2;
        } else {
            // 두 번째 삼각형
            if (vertexInQuad == 3.0) pos = p2;
            if (vertexInQuad == 4.0) pos = p1;
            if (vertexInQuad == 5.0) pos = p3;
        }
    
        // 소용돌이 확장 (NDC 전체 덮음)
        vec2 primarySwirl = swirl(pos * 1.5, vec2(0.0, 0.0), 6.0, time * 0.5);
    
        // 다중 별 설정
        vec2 starCenters[15] = vec2[15](
            vec2(0.0, 0.4), vec2(-0.6, 0.7), vec2(0.5, 0.5), vec2(-0.8, -0.2), vec2(0.7, -0.3), 
            vec2(-0.3, -0.5), vec2(0.6, 0.8), vec2(-0.9, 0.2), vec2(0.4, -0.6), vec2(-0.4, 0.1),
            vec2(0.2, -0.8), vec2(-0.6, 0.3), vec2(0.8, 0.9), vec2(-0.7, -0.7), vec2(0.1, 0.2)
        );
        float starRadii[15] = float[15](0.25, 0.2, 0.22, 0.18, 0.2, 0.15, 0.18, 0.17, 0.14, 0.16, 0.13, 0.19, 0.15, 0.12, 0.21);
        float starIntensities[15] = float[15](8.0, 7.0, 7.5, 6.0, 6.5, 5.0, 6.5, 7.2, 5.5, 5.8, 6.0, 7.0, 6.2, 5.5, 6.8);
    
        vec3 starColors = (0.1 + snd * 3.0) * multipleStars(primarySwirl, starCenters, starRadii, starIntensities);
    
        // 구름과 배경 색상
        float height = noise(primarySwirl * 5.0 + time * 0.1) * 0.1;
        vec3 cloudColor = mix(
            vec3(0.1, 0.1, 0.3),  // 어두운 남색 구름
            vec3(0.3, 0.4, 0.6),  // 은은한 파란 구름
            height
        ) * 0.4; // 구름 밝기 조정
    
        // 최종 색상 조합
        vec3 finalColor = background.rgb * 0.3 + cloudColor + starColors * (0.5 + volume / 3.0);
    
        // 색상 클램핑
        finalColor = clamp(finalColor, 0.0, 1.0);
    
        // 빛나는 효과 추가
        finalColor += vec3(0.03 * abs(sin(time * 2.0 + vertexId * 0.1)));
    
        // 출력 색상
        v_color = vec4(finalColor, 1.0);
    
        // gl_Position 설정
        gl_Position = vec4(primarySwirl * vec2(aspect, 1.0), height, 1.0);
    }
      `;

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