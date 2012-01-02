// The Identity Shader
// Fragment Shader
// Richard S. Wright Jr.
// OpenGL SuperBible
#version 150

in vec2 vTex;

out vec4 vFragColor;

uniform vec4 vColor;
uniform sampler2D textureUnit0;

void main(void)
{ 
   vFragColor = vColor * texture(textureUnit0, vTex);
}

