#version 330 core

in vec3 ourColor;
out vec4 FragColor;

uniform vec3 color;
uniform bool override;

void main()
{
    if(override){
        FragColor = vec4(color, 1.0);
    }
    else{
        FragColor = vec4(ourColor, 1.0);
    }
}
