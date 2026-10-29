#version 330 core

in vec2 vUV;
out vec4 FragColor;

uniform vec3 glowColor;
uniform float glowScale;
uniform float intensity;

void main(){
    float r = max(length(vUV) * glowScale, 1.0);

    float halo = 0.25 / (r * r);
    float rim = 0.55 * exp(-3.0 * (r - 1.0));
    float fade = 1.0 - smoothstep(0.6 * glowScale, glowScale, r);

    FragColor = vec4(glowColor * (halo + rim) * fade * intensity, 1.0);
}