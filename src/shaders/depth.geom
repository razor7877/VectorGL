#version 410 core

layout (triangles, invocations = 5) in;
layout (triangle_strip, max_vertices = 3) out;

uniform mat4 lightSpaceMatrices[4];

void main()
{
    for (int i = 0; i < 4; i++)
    {
        gl_Position = lightSpaceMatrices[gl_InvocationID] * gl_in[i].gl_Position;
        gl_Layer = gl_InvocationID;
        EmitVertex();
    }
    EndPrimitive();
}