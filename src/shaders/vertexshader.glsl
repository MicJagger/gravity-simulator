#version 330 core

float zNear = 0.0000000001;
float zFar = 1000000000.0;
float fCoeff = 1.0 / log2(zFar + 1.0);

uniform mat4 projectionMatrix;
uniform mat4 viewMatrix;

layout (location = 0) in vec3 vPos;
layout (location = 1) in vec3 vColor;
layout (location = 2) in vec2 vTexCoords;
layout (location = 3) in float vMinBrightness;
layout (location = 4) in vec3 vNormals;

out vec3 vertexPos;
out vec3 vertexColor;
out vec2 vertexTexCoords;
out float vertexMinBrightness;
out vec3 vertexNormal;
out float fragDepth;

void main() {
    gl_Position = projectionMatrix * viewMatrix * vec4(vPos.x, vPos.y, vPos.z, 1.0f);
    gl_Position.z = log2(max(zNear, 1.0 + gl_Position.w)) * fCoeff * 2.0 - 1.0;
    fragDepth = log2(1.0 + gl_Position.w) * fCoeff;
    vertexPos = vPos;
    vertexColor = vColor;
    //vertexTexCoords = vec2(vTexCoords.x, vTexCoords.y * -1.0f);
    vertexMinBrightness = vMinBrightness;
    vertexNormal = vNormals;
}
