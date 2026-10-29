#version 330 core
in vec3 vNormal;
in vec3 vViewSpacePos;
in vec3 vViewSpaceNormal;

out vec4 FragColor;

//uniform vec3 sunColor;
uniform vec3 coreColor;
uniform vec3 edgeColor;
uniform float intensity;

void main()
{
    vec3 N = normalize(vViewSpaceNormal);
    vec3 V = normalize(-vViewSpacePos);
    float mu = clamp(dot(N, V), 0.0, 1.0);

    // Only blue changes across the disc: ~0.5 in the centre, ~0.05 at the edge
    vec3 color = mix(edgeColor, coreColor, mu * mu);
    color.b += 0.3 * pow(mu, 40.0);
    FragColor = vec4(color, 1.0);
}