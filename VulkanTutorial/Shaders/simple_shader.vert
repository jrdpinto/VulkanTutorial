#version 450

// Vertex output colour
layout(location = 0) out vec3 fragColour;

vec3 positions[3] = vec3[]
(
    vec3(0.0, -0.5, 0.0),
    vec3(0.5,  0.5, 0.0),
    vec3(-0.5, 0.5, 0.0)
);

vec3 colours[3] = vec3[](
    vec3(1.0, 0.0, 0.0),
    vec3(0.0, 1.0, 0.0),
    vec3(0.0, 0.0, 1.0)
);

void main()
{
    gl_Position = vec4(positions[gl_VertexIndex], 1.0);
    fragColour = colours[gl_VertexIndex];
}