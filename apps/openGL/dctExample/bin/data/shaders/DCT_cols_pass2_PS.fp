#version 150
#define DEBUG 0

in vec2 texcoord;
out vec4 fragColor;
uniform sampler2D image;
uniform float width;


// split into even and odd passes:
vec4 DCT_odd(float d[8])
{
	float tmp4, tmp5, tmp6, tmp7;
	float tmp10, tmp11, tmp12;
	float z2, z3, z4, z5, z11, z13;
	vec4 doutput;
	
    tmp7 = d[0] - d[7];
    tmp6 = d[1] - d[6];
    tmp5 = d[2] - d[5];
    tmp4 = d[3] - d[4];

    /* Odd part */
    tmp10 = tmp4 + tmp5;	/* phase 2 */
    tmp11 = tmp5 + tmp6;
    tmp12 = tmp6 + tmp7;

    /* The rotator is modified from fig 4-8 to avoid extra negations. */
    z5 = (tmp10 - tmp12) * 0.382683433; /* c6 */
    z2 = 0.541196100 * tmp10 + z5; /* c2-c6 */
    z4 = 1.306562965 * tmp12 + z5; /* c2+c6 */
    z3 = tmp11 * 0.707106781; /* c4 */

    z11 = tmp7 + z3;		/* phase 5 */
    z13 = tmp7 - z3;

    doutput[0] = z13 + z2;	/* phase 6 */ 	// dataptr[5]
    doutput[1] = z13 - z2;					// dataptr[3]
    doutput[2] = z11 + z4;					// dataptr[1]
    doutput[3] = z11 - z4;					// dataptr[7]

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
    fragColor = vec4(d[5], d[3], d[1], d[7]);
#else
	fragColor = DCT_odd(d);
#endif
}




