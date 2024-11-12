#version 330 core
in vec3 color;
in vec3 fragPos;

out vec4 FragColor;

uniform vec3 cameraPos;

void main() {
    float distance = length(cameraPos - fragPos);
    float alpha = smoothstep(0.f, 1.f, distance);
    FragColor = vec4(color, alpha);
}
