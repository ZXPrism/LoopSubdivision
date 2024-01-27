#version 330 core

layout(location = 0) in vec3 _coords;

uniform mat4 mvp;

void main() {
    gl_Position = mvp * vec4(_coords, 1.0);
}
