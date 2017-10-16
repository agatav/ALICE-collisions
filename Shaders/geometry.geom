#version 420 core

layout (lines_adjacency) in;
layout (line_strip, max_vertices = 4) out;

in VS_OUT {
    vec2 color;
} gs_in[];

out vec2 fColor;

void main(void)
{

    fColor = gs_in[0].color;
    int i;

    for (i = 0; i < gl_in.length(); i++)
    {
        gl_Position = gl_in[i].gl_Position;
        EmitVertex();
    }
}

//
//layout (lines_adjacency) in;
//layout (line_strip, max_vertices = 64) out;
//
//
//in VS_OUT {
//    vec2 color;
//} gs_in[];
//
//out vec2 fColor;
//const float PI = 3.1415926;
//
//void build_house(vec4 position)
//{
//   fColor = gs_in[0].color; // gs_in[0] since there's only one input vertex
//
//      for (int i = 0; i <= 32; i++) {
//               // Angle between each side in radians
//               float ang = PI * 2.0 / 32 * i;
//
//               // Offset from center of point (0.3 to accomodate for aspect ratio)
//               vec4 offset = vec4(cos(ang)*0.3, -sin(ang)*0.4, 0.0, 0.0);
//               gl_Position = position + offset;
//
//               EmitVertex();
//               }
//    EndPrimitive();
//}
//
//void main() {
//    build_house(gl_in[0].gl_Position);
//}