#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in mat4 instanceTransform;
layout (location = 5) in vec3 instanceColor;

uniform mat4 projection;
uniform mat4 view;

out vec3 color;

void main()
{
    gl_Position = projection * view * instanceTransform * vec4(aPos, 1.0);
    
    color = instanceColor;
}
