//
//  LabAssignment1.h
//  MatrixFunctions
//
//  Created by Sam Nosenzo on 9/16/17.
//  Copyright © 2017 Sam Nosenzo. All rights reserved.
//

#ifndef LabAssignment1_h
#define LabAssignment1_h
#include <OpenGL/OpenGL.h>
#include <GLUT/glut.h>
#include <stdio.h>


#endif /* LabAssignment1_h */

typedef struct vec4
{
    GLfloat x;
    GLfloat y;
    GLfloat z;
    GLfloat w;
} vec4;

typedef struct mat4
{
    vec4 v0;
    vec4 v1;
    vec4 v2;
    vec4 v3;
    
} mat4;

void defineVector(GLfloat, GLfloat, GLfloat, GLfloat w, vec4*);

void printVector(vec4*);

void crossProduct(vec4* vec1, vec4* vec2, vec4* res);

float magnitude(vec4 * v);

GLfloat dotProduct(vec4* vec1, vec4* vec2);

void scalarMultVector(vec4* vec, GLfloat scalar, vec4* res);

void addVectors(vec4* vec1, vec4* vec2, vec4* res);

void subVectors(vec4* vec1, vec4* vec2, vec4* res);

void multVectors(vec4* vec1, vec4* vec2, vec4* res);

// MATRIX FUNCTIONS

void defineMatrix(vec4 v0, vec4 v1, vec4 v2, vec4 v3, mat4* res);

void printMatrix(mat4* m);

void scalarMultMatrix(mat4* m, GLfloat scalar, mat4* res);

void addMatrix(mat4* m1, mat4* m2, mat4* res);

void subMatrix(mat4* m1, mat4* m2, mat4* res);

void transposeMatrix(mat4* m, mat4* trans);

void cofactorMatrix(mat4* m, mat4* cofactor);

GLfloat determinantMatrix(mat4* m, mat4* minor);

GLfloat absoluteValue(GLfloat a);

void inverseMatrix(mat4* m, mat4* res);

void multiplyMatrices(mat4 *m1, mat4 *m2, mat4 *res);

void multMatrixVector(mat4 *m, vec4 *v, vec4 *res);

