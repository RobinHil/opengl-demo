#version 330 core
layout (triangles) in;
layout (line_strip, max_vertices = 6) out;

in VS_OUT {
    vec3 normal;
} gs_in[];

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;
uniform float normalLength;

void GenerateLine(int index) {
    vec4 worldPos = gl_in[index].gl_Position;
    gl_Position = worldPos;
    EmitVertex();
    
    vec3 normalDir = normalize(mat3(transpose(inverse(model))) * gs_in[index].normal);
    vec4 endPoint = worldPos + vec4(normalDir * normalLength, 0.0);
    gl_Position = endPoint;
    EmitVertex();
    
    EndPrimitive();
}

void main() {
    GenerateLine(0);
    GenerateLine(1);
    GenerateLine(2);
}