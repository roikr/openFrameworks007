#version 150
#define DEBUG 0

in vec2 texcoord;
out vec4 fragColor;
uniform sampler2D image;
uniform float width;


// split into even and odd passes:
vec4 DCT_even(float d[8])
{
	float tmp0, tmp1, tmp2, tmp3;
	float tmp10, tmp11, tmp12, tmp13;
	float z1;
	vec4 doutput;
	
    tmp0 = d[0] + d[7];
    tmp1 = d[1] + d[6];
    tmp2 = d[2] + d[5];
    tmp3 = d[3] + d[4];
    
    /* Even part */
    tmp10 = tmp0 + tmp3;	/* phase 2 */
    tmp13 = tmp0 - tmp3;
    tmp11 = tmp1 + tmp2;
    tmp12 = tmp1 - tmp2;
    
    doutput[0] = tmp10 + tmp11; /* phase 3 */	// dataptr[0]
    doutput[1] = tmp10 - tmp11;					// dataptr[4]
    
    z1 = (tmp12 + tmp13) * 0.707106781; /* c4 */
    doutput[2] = tmp13 + z1;	/* phase 5 */		// dataptr[2]
    doutput[3] = tmp13 - z1;						// dataptr[6]

	return doutput;
}



void main(void)
{ 
    // read row of 8 pixels
	float d[8];
    float x = 1.0/width;
    for(int i=0; i<8; i++) {
        d[i] = texture(image,texcoord*vec2(1.0, 8.0) + vec2(0, i*x)).x;
	}
    

#if DEBUG
    fragColor = vec4(d[0], d[4], d[2], d[6]);
#else
	fragColor = DCT_even(d);
#endif
}
