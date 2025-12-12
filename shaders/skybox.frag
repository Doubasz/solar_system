#version 330 core
in vec3 viewDir;
out vec4 FragColor;

float hash13(vec3 p3){
    p3 = fract(p3 * 0.1031);
    p3 += dot(p3, p3.zyx + 31.32);
    return fract((p3.x + p3.y) * p3.z);
}

vec3 hash33(vec3 p3) {
    p3 = fract(p3 * vec3(0.1031, 0.1030, 0.0973));
    p3 += dot(p3, p3.yxz + 33.33);
    return fract((p3.xxy + p3.yxx) * p3.zyx);
}

void main() {
    vec3 dir = normalize(viewDir);
    vec3 p = floor(dir * 150.0);

    float h = hash13(p);

    if(h < 0.250) {
        FragColor = vec4(0.0, 0.0, 0.01, 1.0);
        return;
    }

    vec3 fp = fract(dir * 150.0);
    vec3 sp = hash33(p);
    float d = length(fp - sp);

    float intensity = smoothstep(0.15, 0.0, d) * pow(hash13(p + vec3(13.7)), 2.0);

    FragColor = vec4(vec3(intensity), 1.0);
}
