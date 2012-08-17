// The Identity Shader
// Fragment Shader
// Richard S. Wright Jr.
// OpenGL SuperBible
#version 150

in vec2 vTex;

out vec4 vFragColor;

uniform vec4 vColor;
uniform sampler2D depthTexture;
uniform sampler2D colorTexture;

void main(void)
{ 
   vec3 depth = vColor.rgb * texture(depthTexture, vTex).r;
   vec3 color = texture(colorTexture, vTex).rgb;
   vFragColor.rgb =mix(depth,color,0.8);
   vFragColor.a = vColor.a;
}

