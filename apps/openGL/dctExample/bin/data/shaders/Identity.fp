// The Identity Shader
// Fragment Shader
// Richard S. Wright Jr.
// OpenGL SuperBible
#version 150

in vec2 vTex;

out vec4 fragColor;

uniform sampler2D image;

void main(void)
{ 
   fragColor = texture(image, vTex).r * vec4(1,1,1,1);
}

