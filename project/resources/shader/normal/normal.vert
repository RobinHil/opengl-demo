#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

out VS_OUT {
    vec3 normal;
} vs_out;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform float normalLength;

void main() {
    vec3 worldPos = vec3(model * vec4(aPos, 1.0));
    vs_out.normal = aNormal;
    gl_Position = projection * view * vec4(worldPos, 1.0);
}