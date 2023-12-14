#version 330 core

in vec3 nearPoint;
in vec3 farPoint;

out vec4 FragColor;

void main() {
    FragColor = vec4(1.0, 0.0, 0.0, 1.0);
}