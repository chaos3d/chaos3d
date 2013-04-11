//
//  Shader.fsh
//  Minimalist
//
//  Created by Hao Wu on 4/11/13.
//  Copyright (c) 2013 cHaos3D. All rights reserved.
//

varying lowp vec4 colorVarying;

void main()
{
    gl_FragColor = colorVarying;
}
