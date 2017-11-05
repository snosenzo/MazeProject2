//
//  viewFuncs.c
//  MazeProject2
//
//  Created by Sam Nosenzo on 10/31/17.
//  Copyright © 2017 Sam Nosenzo. All rights reserved.
//

#include "viewFuncs.h"

void look_at(float eyex, float eyey, float eyez, float atx, float aty, float atz, float upx, float upy, float upz, mat4* res){
    mat4 rot, trans;
    vec4 eye, at, up, n, u, v, vpn, temp, v0, v1, v2, v3;
    
    defineVector(eyex, eyey, eyez, 1, &eye);
    defineVector(atx, aty, atz, 1, &at);
    defineVector(upx, upy, upz, 1, &up);
    
    subVectors(&eye, &at, &vpn);
    scalarMultVector(&vpn, 1.0f/magnitude(&vpn), &n);
    
    crossProduct(&up, &n, &temp);
    scalarMultVector(&temp, 1.0f/magnitude(&temp), &u);
    
    crossProduct(&n, &u, &temp);
    scalarMultVector(&temp, 1.0f/magnitude(&temp), &v);
    
    defineVector(u.x, v.x, n.x, 0, &v0);
    defineVector(u.y, v.y, n.y, 0, &v1);
    defineVector(u.z, v.z, n.z, 0, &v2);
    defineVector(eyex, eyey, eyez, 1, &v3);
   
    defineMatrix(v0, v1, v2, v3, &temp);
    transposeMatrix(&temp, &rot);
    inverseMatrix(&rot, res);
    
//    getTranslationMatrix(eye.x, eye.y, eye.z, &trans);
//    multiplyMatrices(&rot, &trans, res);
}
void frustum(float left, float right, float bottom, float top, float near, float far, mat4* res) {
    vec4 v0, v1, v2, v3;
    defineVector((near/right), 0, 0, 0, &v0);
    defineVector(0, (near/top), 0, 0, &v1);
    defineVector(0, 0, -1*(near + far)/(far-near), 0, &v2);
    defineVector(0, 0, -1*(2*near*far)/(far-near), -1, &v3);
    defineMatrix(v0, v1, v2, v3, res);
    printMatrix(res);
}

void perspective(float right, float top, float near, float far, mat4 *res) {
    vec4 v0, v1, v2, v3;
    defineVector(near/right, 0.0, 0.0, 0.0, &v0);
    defineVector(0.0, near/top, 0.0, 0.0, &v1);
    defineVector(0.0, 0.0, -1.0*(far + near)/(far - near), -1.0, &v2);
    defineVector(0.0, 0.0, -2.0*far*near/(far - near), 0.0, &v3);
    defineMatrix(v0, v1, v2, v3, res);
}

void look_at_theta(float theta, float y, mat4 *res){
    float eyex, eyez;
    eyez = cosf(theta);
    eyex = sinf(theta);
    printf("eyex: %f, eyey: %f, eyez: %f/n", eyex, y, eyez);
    
    look_at(eyex, y, eyez, 0, 0, 0, 0, 1, 0, res);
    
}

