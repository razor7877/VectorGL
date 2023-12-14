#version 330 core

// Shared set between most vertex shaders
layout (std140) uniform Matrices
{
    mat4 view;
    mat4 projection;
};

out vec3 nearPoint;
out vec3 farPoint;

// Grid position are in xy clipped space
vec3 gridPlane[6] = vec3[]
(
    vec3(1, 1, 0), vec3(-1, -1, 0), vec3(-1, 1, 0),
    vec3(-1, -1, 0), vec3(1, 1, 0), vec3(1, -1, 0)
);

vec3 UnprojectPoint(float x, float y, float z, mat4 view, mat4 projection)
{
    mat4 viewInv = inverse(view);
    mat4 projInv = inverse(projection);
    vec4 unprojectedPoint =  viewInv * projInv * vec4(x, y, z, 1.0);
    return unprojectedPoint.xyz / unprojectedPoint.w;
}

// normal vertice projection
void main()
{
    vec3 p = gridPlane[gl_VertexID].xyz;
    nearPoint = UnprojectPoint(p.x, p.y, 0.0, view, projection);
    farPoint = UnprojectPoint(p.x, p.y, 1.0, view, projection);

    gl_Position = vec4(p, 1.0);
}