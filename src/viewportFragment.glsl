#version 410 core
out vec4 fragColor;

uniform int windowWidth;
uniform int windowHeight;

void main(){
    vec2 normalized = vec2(gl_FragCoord.x / windowWidth, gl_FragCoord.y / windowHeight);
    fragColor = vec4(normalized, 0.0, 0.0);
}
