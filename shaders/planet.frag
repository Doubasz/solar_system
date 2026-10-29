#version 330 core

in vec3 vNormal;       // interpolated normal in world space
in vec3 vPosition;     // world-space position of the fragment

out vec4 FragColor;

uniform vec3 sunPos;       // sun position in world space
uniform vec3 sunColor;     // sun light color
uniform float sunRadius;
uniform float sunIntensity; // light intensity

uniform vec3 cameraPos;    // camera position
uniform vec3 planetColor;  // base color of the planet
uniform vec3 overrideColor;
uniform bool override;


void main()
{
    if(override){
        FragColor = vec4(overrideColor, 1.0);
    }
    else{

        // Normalize vectors
        vec3 N = normalize(vNormal);
        vec3 L = normalize(sunPos - vPosition); // light direction
        vec3 V = normalize(cameraPos - vPosition); // view direction
        //vec3 R = reflect(-L, N); // reflection for specular

        vec3 light = sunColor * sunIntensity;

        // --- Diffuse ---
        float NdotL = max(dot(N, L), 0.0);
        vec3 diffuse = NdotL * light * planetColor;

        // --- Specular (Blinn-Phong) ---
        float shininess = 32.0;
        float specularStrength = 0.25;
        vec3 H = normalize(L + V); // half vector
        float spec = (NdotL > 0.0) ? pow(max(dot(N, H), 0.0), shininess) : 0.0;
        vec3 specular = specularStrength * spec * light;

        float ambientStrength = 0.03;
        vec3 ambient = ambientStrength * planetColor;

        //ds = vec3(1.0, 1.0, 1.0);

        // --- Combine ---
        vec3 color = ambient + diffuse + specular;
        //color = pow(color, vec3(1.0/2.2));

        FragColor = vec4(color, 1.0);
    }
}
