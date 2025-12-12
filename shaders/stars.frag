#version 330 core

in vec3 vNormal;
in vec3 vPosition;

out vec4 FragColor;


void main() {
    vec3 dir = normalize(viewDir);
    vec3 p = floor(dir * 150.0);
    
    float h = hash13(p);
    
    // Quick rejection
    if(h < 0.996) {
        FragColor = vec4(0.0, 0.0, 0.01, 1.0);
        return;
    }
    
    // Calculate star
    vec3 fp = fract(dir * 150.0);
    vec3 sp = hash33(p);
    float d = length(fp - sp);
    
    float intensity = smoothstep(0.15, 0.0, d) * pow(hash13(p + vec3(13.7)), 2.0);
    
    FragColor = vec4(vec3(intensity), 1.0);
}