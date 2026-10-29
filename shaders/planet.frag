#version 330 core

in vec3 vNormal;       // interpolated normal in world space
in vec3 vPosition;     // world-space position of the fragment

out vec4 FragColor;

uniform vec3 sunPos;       // sun position in world space
uniform vec3 sunColor;     // sun light color
uniform float sunIntensity; // light intensity

uniform vec3 cameraPos;    // camera position
uniform vec3 planetColor;  // base color of the planet
uniform vec3 ambientColor;
uniform vec3 rimColor;
uniform float wrap;

uniform vec3 overrideColor;
uniform bool override;


void main()
{
    if(override){
        FragColor = vec4(overrideColor, 1.0);
        return;
    }

    // Normalize vectors
    vec3 N = normalize(vNormal);
    vec3 L = normalize(sunPos - vPosition); // light direction
    vec3 V = normalize(cameraPos - vPosition); // view direction

    vec3 light = sunColor * sunIntensity;

    float NdotL = dot(N, L);
    float NdotV = max(dot(N, V), 0.0);

    // Wrap lighting: light reaches past the 90° terminator, giving a long, soft gradient
    float diff = max((NdotL + wrap) / (1.0 + wrap), 0.0);
    diff *= diff;                       // squared: starts gently instead of with a visible line
    vec3 diffuse = diff * light * planetColor;

    float rim = pow(1.0 - NdotV, 3.0) * smoothstep(-0.3, 0.6, NdotL);
    vec3 rimLight = rim * rimColor;
    

    vec3 color = ambientColor + diffuse + rimLight;        
    FragColor = vec4(color, 1.0);
    
}
