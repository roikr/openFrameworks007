//
//  Shader.fsh
//  emptyExample
//
//  Created by Roee Kremer on 5/13/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

varying lowp vec4 colorVarying;

void main()
{
    gl_FragColor = colorVarying;
}
