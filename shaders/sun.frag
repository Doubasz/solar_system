#version 330 core
in vec3 vNormal;
in vec3 vViewSpacePos;
in vec3 vViewSpaceNormal;

out vec4 FragColor;

uniform vec3 sunColor;
uniform float intensity;

void main()
{
    // View direction in view space (camera at origin looking down -Z)
    vec3 viewDir = normalize(-vViewSpacePos);
    
    // How much this fragment faces the camera (1.0 = directly facing, 0.0 = edge)
    float NdotV = max(dot(normalize(vViewSpaceNormal), viewDir), 0.0);
    
    // Create multiple glow layers with different falloffs
    float innerCore = pow(NdotV, 2.0);     // Sharp bright center
    float outerCore = pow(NdotV, 2.0);     // Medium falloff
    float corona = pow(NdotV, 8.0);        // Soft outer glow
    float atmosphere = pow(NdotV, 8.0);   // Very soft halo
    
    // Blend layers together
    float brightness = innerCore * 1.2 + 
                      outerCore * 0.5 + 
                      corona * 0.15 + 
                      atmosphere * 0.15;
    
    // Color gradient from edge to center
    vec3 hotCenter = vec3(1.0, 0.6, 0.1);
    vec3 warmEdge = sunColor;
    vec3 color = mix(warmEdge, hotCenter, innerCore);


    //float alpha = pow(NdotV, 1.0);  // tweak this value!
    
    FragColor = vec4(color * brightness * intensity, 1.0);
}