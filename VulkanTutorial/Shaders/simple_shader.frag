#version 450

// Interpolated colour from vertex (location must match)
layout(location = 0) in vec3 fragColour;

// Final output colour (must also have location)
layout(location = 0) out vec4 outColour;

void main() 
{
    outColour = vec4(fragColour, 1.0);
} 