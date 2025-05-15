#version 330 core

uniform vec3 lightPos;

in vec3 vertexPos;
in vec3 vertexColor;
in float vertexMinBrightness;
in vec3 vertexNormal;
in float fragDepth;

out vec4 FragColor;

void main() {
    vec3 vNorm = normalize(vertexNormal);
    vec3 lightDirection = normalize(lightPos - vertexPos);
    float diff = max(dot(vNorm, lightDirection), 0.0f);
    float lightIntensity = min(diff + vertexMinBrightness, 1.0f);
    FragColor = vec4(vertexColor * lightIntensity, 1.0f);
    gl_FragDepth = fragDepth;
}
