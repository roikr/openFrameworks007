#version 150

//precision highp float;
	
uniform sampler2D uSampler;

in vec4 vColor;
in vec4 vTextureCoord;

out vec4 out_FragColor;

void main(void) {
    // -- get the pixel from the texture
    //vec4 textureColor = texture2D(uSampler, vec2(vTextureCoord.s, vTextureCoord.t));
    // -- multiply the texture pixel with the vertex color
    vec4 textureColor = texture(uSampler, vTextureCoord.st);
    out_FragColor = vColor * textureColor;//* texture(uSampler, vTextureCoord.st);// * textureColor;
}