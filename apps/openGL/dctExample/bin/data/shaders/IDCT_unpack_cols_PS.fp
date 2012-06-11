#version 150
// convert greyscale image packed into 2 RGBA textures into a single image 8 times as wide


in vec2 texcoord;
out vec4 fragColor;
uniform sampler2D image;
uniform sampler2D image2;
uniform float width;


void main(void)
{ 
   
    vec2 uv = texcoord * vec2(1.0,1.0/8.0);
	vec4 c = texture(image, uv);
	vec4 c2 = texture(image2, uv);
	
	// rearrange data into correct order
	//    x y z w
	// c  0 4 2 6
	// c2 5 3 1 7
    
    float f=fract(texcoord.y*width/8);// * 8.0;
    ivec4 i=int(floor(8*f))*ivec4(1,1,1,1);
    
	bvec4 sel0 = equal(i,ivec4(0, 1, 2, 3));
	bvec4 sel1 = equal(i,ivec4(4, 5, 6, 7));

    
    fragColor.r = dot(c,vec4(sel0))+dot(c2,vec4(sel1)); 
    fragColor.b = 0;
    fragColor.g = 0;
    fragColor.a = 0;

}