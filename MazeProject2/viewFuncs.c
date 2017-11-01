//
//  viewFuncs.c
//  MazeProject2
//
//  Created by Sam Nosenzo on 10/31/17.
//  Copyright © 2017 Sam Nosenzo. All rights reserved.
//

#include "viewFuncs.h"

void look_at(float eyex, float eyey, float eyez, float atx, float aty, float atz, float upx, float upy, float upz, mat4* res){
    
}
void frustum(float left, float right, float bottom, float top, float near, float far, mat4* res) {
    vec4 v0, v1, v2, v3;
    defineVector(-1 * (near/right), 0, 0, 0, &v0);
    defineVector(0, -1 * (near/top), 0, 0, &v1);
    defineVector(0, 0, (near + far)/(far-near), -1, &v2);
    defineVector(0, 0, (2*near*far)/(far-near), 0, &v3);
    defineMatrix(v0, v1, v2, v3, res);
}


