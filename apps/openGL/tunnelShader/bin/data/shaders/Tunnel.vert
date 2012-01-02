#version 150

// -- "attribute": read-only per-vertex data, available only within vertex shaders.
// -- the vertex position (x, y, z)
in vec4 aVertexPosition;
// -- the vertex color (r, g, b, a)
in vec4 aVertexColor;
// -- the texture coordinate for this vertex (u, v)
in vec4 aTextureCoord;

// -- "uniform": remains constant during each shader execution.
// -- model-view matrix
uniform mat4 uMVMatrix;
// -- projection matrix
uniform mat4 uPMatrix;
// -- the time value (changes every frame)
uniform float fTime;

// -- "varying": output of the vertex shader that corresponds to read-only interpolated input
//    of the fragment shader
// -- the color
out vec4 vColor;
// -- the texture coordinates
out vec4 vTextureCoord;

void main(void) {
    vec4 pos=aVertexPosition; 
    // -- displace the x coordinate based on the time and the z position 
    pos.x += cos(fTime + (aVertexPosition.z/4.0)); 
    // -- displace the y coordinate based on the time and the z position 
    pos.y += sin(fTime + (aVertexPosition.z/4.0)); 
    // -- transform the vertex 
    //gl_Position = uPMatrix * uMVMatrix * vec4(pos, 1.0); 
    gl_Position = uPMatrix * uMVMatrix * pos; 
    // -- copy the vertex color
    vColor = aVertexColor; 
    // -- displace the texture's y (v) coordinate. This gives the illusion of movement.
    vec4 texcoord=aTextureCoord; 
    texcoord.y = texcoord.y + (fTime); 
    // -- copy the texture coordinate 
    vTextureCoord = texcoord; 
}