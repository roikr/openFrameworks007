#version 150
#define DEBUG 0

in vec2 texcoord;
out vec4 fragColor;
uniform sampler2D image;
uniform float width;

vec4 IDCT_2(float d[8])
{
	float tmp0, tmp1, tmp2, tmp3, tmp4, tmp5, tmp6, tmp7;
	float tmp10, tmp11, tmp12, tmp13;
	float z5, z10, z11, z12, z13;
	vec4 doutput;
	
    /* Even part */
    tmp0 = d[0];
    tmp1 = d[2];
    tmp2 = d[4];
    tmp3 = d[6];

    tmp10 = tmp0 + tmp2;	/* phase 3 */
    tmp11 = tmp0 - tmp2;

    tmp13 = tmp1 + tmp3;	/* phases 5-3 */
    tmp12 = (tmp1 - tmp3) * 1.414213562 - tmp13; /* 2*c4 */

    tmp0 = tmp10 + tmp13;	/* phase 2 */
    tmp3 = tmp10 - tmp13;
    tmp1 = tmp11 + tmp12;
    tmp2 = tmp11 - tmp12;
    
    /* Odd part */
    tmp4 = d[1];
    tmp5 = d[3];
    tmp6 = d[5];
    tmp7 = d[7];

    z13 = tmp6 + tmp5;		/* phase 6 */
    z10 = tmp6 - tmp5;
    z11 = tmp4 + tmp7;
    z12 = tmp4 - tmp7;

    tmp7 = z11 + z13;		/* phase 5 */
    tmp11 = (z11 - z13) * 1.414213562; /* 2*c4 */

    z5 = (z10 + z12) * 1.847759065; /* 2*c2 */
    tmp10 = 1.082392200 * z12 - z5; /* 2*(c2-c6) */
    tmp12 = -2.613125930 * z10 + z5; /* -2*(c2+c6) */

    tmp6 = tmp12 - tmp7;	/* phase 2 */
    tmp5 = tmp11 - tmp6;
    tmp4 = tmp10 + tmp5;

 // 	output[0] = tmp0 + tmp7;		// wsptr[DCTSIZE*0]
    doutput[3] = tmp0 - tmp7;		// wsptr[DCTSIZE*7]
 //   output[1] = tmp1 + tmp6;		// wsptr[DCTSIZE*1]
    doutput[2] = tmp1 - tmp6;		// wsptr[DCTSIZE*6]
 //   output[2] = tmp2 + tmp5;		// wsptr[DCTSIZE*2]
    doutput[1] = tmp2 - tmp5;		// wsptr[DCTSIZE*5]
    doutput[0] = tmp3 + tmp4;		// wsptr[DCTSIZE*4]
//    output[3] = tmp3 - tmp4;		// wsptr[DCTSIZE*3]
	return doutput;
}

void main(void)
{ 
    // read row of 8 pixels
	float d[8];
    float x = 1.0/width;
    for(int i=0; i<8; i++) {
        d[i] = texture(image,texcoord*vec2(8.0, 1.0) + vec2(i*x, 0)).x;
	}
    

#if DEBUG
    fragColor = vec4(d[4], d[5], d[6], d[7]);
#else
	fragColor = IDCT_2(d)/8.0;
#endif
}




