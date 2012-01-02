// See http://www.iquilezles.org/articles/menger/menger.htm for the 
// full explanation of how this was done
#version 150

precision highp float;

uniform vec2 resolution;
uniform float time;

out vec4 out_FragColor;

float maxcomp(in vec3 p ) { return max(p.x,max(p.y,p.z));}
float sdBox( vec3 p, vec3 b )
{
  vec3  di = abs(p) - b;
  float mc = maxcomp(di);
  return min(mc,length(max(di,0.0)));
}

vec4 map( in vec3 p )
{
   float d = sdBox(p,vec3(1.0));
   vec4 res = vec4( d, 1.0, 0.0, 0.0 );

   float s = 1.0;
   for( int m=0; m<3; m++ ) 
   {
      vec3 a = mod( p*s, 2.0 )-1.0;
      s *= 3.0;
      vec3 r = abs(1.0 - 3.0*abs(a));

      float da = max(r.x,r.y);
      float db = max(r.y,r.z);
      float dc = max(r.z,r.x);
      float c = (min(da,min(db,dc))-1.0)/s;

      if( c>d )
      {
          d = c;
          res = vec4( d, 0.2*da*db*dc, (1.0+float(m))/4.0, 0.0 );
       }
   }

   return res;
}

// GLSL ES doesn't seem to like loops with conditional break/return...
#if 0
vec4 intersect( in vec3 ro, in vec3 rd )
{
    float t = 0.0;
    for(int i=0;i<64;i++)
    {
        vec4 h = map(ro + rd*t);
        if( h.x<0.002 ) 
            return vec4(t,h.yzw);
        t += h;
    }
    return vec4(-1.0);
}
#else
vec4 intersect( in vec3 ro, in vec3 rd )
{
    float t = 0.0;
    vec4 res = vec4(-1.0);
    for(int i=0;i<64;i++)
    {
        vec4 h = map(ro + rd*t);
        if( h.x<0.002 ) 
        {
            if( res.x<0.0 ) res = vec4(t,h.yzw);
        }
//if( h.x>0.0 )
        t += h.x;
    }
    return res;
}
#endif

vec3 calcNormal(in vec3 pos)
{
    vec3  eps = vec3(.001,0.0,0.0);
    vec3 nor;
    nor.x = map(pos+eps.xyy).x - map(pos-eps.xyy).x;
    nor.y = map(pos+eps.yxy).x - map(pos-eps.yxy).x;
    nor.z = map(pos+eps.yyx).x - map(pos-eps.yyx).x;
    return normalize(nor);
}

void main(void)
{
    vec2 p = -1.0 + 2.0 * gl_FragCoord.xy / resolution.xy;
    p.x *= 1.33;

    // light
    vec3 light = normalize(vec3(1.0,0.8,-0.6));

    float ctime = time;
    // camera
    vec3 ro = 1.1*vec3(2.5*cos(0.5*ctime),1.5*cos(ctime*.23),2.5*sin(0.5*ctime));
    vec3 ww = normalize(vec3(0.0) - ro);
    vec3 uu = normalize(cross( vec3(0.0,1.0,0.0), ww ));
    vec3 vv = normalize(cross(ww,uu));
    vec3 rd = normalize( p.x*uu + p.y*vv + 1.5*ww );

    vec3 col = vec3(0.0);
    vec4 tmat = intersect(ro,rd);
    if( tmat.x>0.0 )
    {
        vec3 pos = ro + tmat.x*rd;
        vec3 nor = calcNormal(pos);

        float dif1 = max(0.4 + 0.6*dot(nor,light),0.0);
        float dif2 = max(0.4 + 0.6*dot(nor,vec3(-light.x,light.y,-light.z)),0.0);

        // shadow
        float ldis = 4.0;
        vec4 shadow = intersect( pos + light*ldis, -light );
        if( shadow.x>0.0 && shadow.x<(ldis-0.01) ) dif1=0.0;

        float ao = tmat.y;
        col  = 1.0*ao*vec3(0.2,0.2,0.2);
        col += 2.0*(0.5+0.5*ao)*dif1*vec3(1.0,0.97,0.85);
        col += 0.2*(0.5+0.5*ao)*dif2*vec3(1.0,0.97,0.85);
        col += 1.0*(0.5+0.5*ao)*(0.5+0.5*nor.y)*vec3(0.1,0.15,0.2);

        // gamma lighting
        col = col*0.5+0.5*sqrt(col)*1.2;

        vec3 matcol = vec3(
            0.6+0.4*cos(5.0+6.2831*tmat.z),
            0.6+0.4*cos(5.4+6.2831*tmat.z),
            0.6+0.4*cos(5.7+6.2831*tmat.z) );
        col *= matcol;
        col *= 1.5*exp(-0.5*tmat.x);
    }


    out_FragColor = vec4(col,1.0);
}