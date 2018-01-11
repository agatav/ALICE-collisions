#version 420 core

layout (lines_adjacency) in;
layout (line_strip, max_vertices = 4) out;

in VS_OUT {
    vec2 color;
} gs_in[];

out vec2 fColor;

void main(void)
{
    int i;
    for (i = 0; i < gl_in.length(); i++)
    {
        gl_Position = gl_in[i].gl_Position;
        fColor = gs_in[i].color;
        EmitVertex();
    }
    EndPrimitive();
}