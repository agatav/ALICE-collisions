#version 420 core
out vec3 FragColor;

in vec2 fColor;

void main()
{
    FragColor = vec3(fColor, 0.6);
}