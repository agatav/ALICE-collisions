#version 420 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aColor;

out VS_OUT {
    vec2 color;
} vs_out;

uniform mat4 transform;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    vs_out.color = aColor;
    gl_Position = projection * view * transform * vec4(aPos.x, aPos.y, aPos.z, 1.0);
}