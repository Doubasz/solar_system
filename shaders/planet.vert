#version 330 core

layout(location=0) in vec3 inPosition; // base sphere position (unit sphere * baseRadius)
layout(location=1) in vec3 inNormal;
layout(location=2) in vec3 inColor;
layout(location=3) in vec2 inTexCoord;

uniform mat4 uModelViewProj;
uniform mat4 uModel;
uniform float uBaseRadius;
uniform float uHeightScale;
uniform float uNoiseFreq;
uniform float uTime; // optional for animated things

out vec3 vWorldPos;
out vec3 vNormal;
out float vElevation;
out vec2 vTex;

vec4 mod289(vec4 x){ return x - floor(x * (1.0/289.0)) * 289.0; }
vec3 mod289(vec3 x){ return x - floor(x * (1.0/289.0)) * 289.0; }
vec4 permute(vec4 x){ return mod289(((x*34.0)+1.0)*x); }

float grad3(vec3 p, vec3 ip) {
    // quick gradient-ish function
    return fract(sin(dot(p, ip) * 12.9898) * 43758.5453) * 2.0 - 1.0;
}

float pnoise(vec3 P) {
    // VERY compact value-noise / pseudo Perlin: acceptable for lookdev
    vec3 Pi = floor(P);
    vec3 Pf = fract(P);
    vec3 w = Pf * Pf * (3.0 - 2.0 * Pf); // smoothstep
    float n000 = grad3(Pf, Pi + vec3(0.0,0.0,0.0));
    float n100 = grad3(Pf - vec3(1.0,0.0,0.0), Pi + vec3(1.0,0.0,0.0));
    float n010 = grad3(Pf - vec3(0.0,1.0,0.0), Pi + vec3(0.0,1.0,0.0));
    float n110 = grad3(Pf - vec3(1.0,1.0,0.0), Pi + vec3(1.0,1.0,0.0));
    float n001 = grad3(Pf - vec3(0.0,0.0,1.0), Pi + vec3(0.0,0.0,1.0));
    float n101 = grad3(Pf - vec3(1.0,0.0,1.0), Pi + vec3(1.0,0.0,1.0));
    float n011 = grad3(Pf - vec3(0.0,1.0,1.0), Pi + vec3(0.0,1.0,1.0));
    float n111 = grad3(Pf - vec3(1.0,1.0,1.0), Pi + vec3(1.0,1.0,1.0));
    float nx00 = mix(n000, n100, w.x);
    float nx01 = mix(n001, n101, w.x);
    float nx10 = mix(n010, n110, w.x);
    float nx11 = mix(n011, n111, w.x);
    float nxy0 = mix(nx00, nx10, w.y);
    float nxy1 = mix(nx01, nx11, w.y);
    float nxyz = mix(nxy0, nxy1, w.z);
    return nxyz;
}

float fbm(vec3 p) {
    float sum = 0.0;
    float amp = 1.0;
    float freq = 1.0;
    for (int i = 0; i < 5; ++i) {
        sum += pnoise(p * freq) * amp;
        freq *= 2.0;
        amp *= 0.5;
    }
    return sum;
}


void main() {
    vec3 dir = normalize((uModel * vec4(inPosition, 1.0)).xyz); // direction in world
    // sample noise in direction space (3D)
    float n = fbm(dir * uNoiseFreq + vec3(uTime * 0.01)); // small animation
    // map roughly [-1,1] to something subtle
    float elevation = n * uHeightScale;
    float r = uBaseRadius + elevation;
    vec3 worldPos = dir * r;
    vWorldPos = worldPos;
    // normal: compute from displaced position (approx)
    vNormal = normalize(worldPos); // OK for low-height-to-radius ratio
    vElevation = elevation;
    vTex = inTexCoord;
    gl_Position = uModelViewProj * vec4(worldPos, 1.0);
}
