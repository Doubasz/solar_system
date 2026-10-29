#version 330 core

layout (location = 0) in vec3 aPos;

out vec2 vUV;

uniform mat4 view;
uniform mat4 projection;
uniform vec3 center; // sun position
uniform float size; // half-width of the quad in world units

void main(){
    vec3 camRight = vec3(view[0][0], view[1][0], view[2][0]);
    vec3 camUp = vec3(view[0][1], view[1][1], view[2][1]);

    vec3 worldPos = center + (camRight * aPos.x + camUp * aPos.y) * size;
    vUV = aPos.xy;
    gl_Position = projection * view * vec4(worldPos, 1.0);
}