//
//  main.c
//  LabRec3
//
//  Created by Sam Nosenzo on 10/25/17.
//  Copyright © 2017 Sam Nosenzo. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <unistd.h>
#include "initShader.h"
#include "transformFuncs.h"
#include "shapeVecs.h"

#define BUFFER_OFFSET( offset )   ((GLvoid*) (offset))

mat4 identity;
vec4* colors;
vec4 total_vertices[1000000];
obj objs[4];
int screenHeight = 512;
int screenWidth = 512;
int bufferCounter = 0;
int num_vertices;
GLuint translate_loc;
GLuint scale_loc;
GLuint rotation_loc;
float thetaX, thetaY, thetaZ;

void initObjs() {
    vec4 v0, v1, v2, v3;
    for(int i = 0; i < 4; i++ ) {
        objs[i].vertices = &cubeVertices;
        objs[i].num_verts = 24;
        num_vertices += 24;
        objs[i].rotation = identity;
    }
    
    getTranslationMatrix(-.5, .5, 0, &(objs[0].translation));
    getTranslationMatrix(.5, .5, 0, &(objs[1].translation));
    getTranslationMatrix(-.5, -.5, 0, &(objs[2].translation));
    getTranslationMatrix(.5, -.5, 0, &(objs[3].translation));
}

void initColors(int num, int numPoints) {
    colors = malloc(sizeof(vec4) * num);
    for(int i = 0; i < num/numPoints; i++) {
        float x, y, z;
        x = (float)rand()/(float)(RAND_MAX);
        y = (float)rand()/(float)(RAND_MAX);
        z = (float)rand()/(float)(RAND_MAX);
        for(int j = 0; j < numPoints; j++) {
            colors[i*numPoints + j].x = x;
            colors[i*numPoints + j].y = y;
            colors[i*numPoints + j].z = z;
            colors[i*numPoints + j].w = 1.0;
        }
    }
}

void loadObjectOnBuffer(obj o, int offset){
    //    glBufferSubData(GL_ARRAY_BUFFER, offset, o.num_verts*sizeof(vec4), o.vertices);
    for(int i = 0; i < o.num_verts; i++ ) {
        total_vertices[i+offset] = o.vertices[i];
    }
    bufferCounter+=sizeof(vec4) * o.num_verts;
    o.buffer_start_loc = offset;
}

void init(void)
{
    thetaX = 0;
    thetaY = 0;
    thetaZ = 0;
    initObjs();
    initColors(num_vertices, 1);
    GLuint program = initShader("vshader.glsl", "fshader.glsl");
    glUseProgram(program);
    GLuint vao;
    glGenVertexArraysAPPLE(1, &vao);
    glBindVertexArrayAPPLE(vao);
    GLuint buffer;
    glGenBuffers(1, &buffer);
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vec4)*num_vertices*2, NULL, GL_STATIC_DRAW);
    
    for(int i = 0; i < 4; i++ ){
        loadObjectOnBuffer(objs[i], i*sizeof(vec4)*objs[i].num_verts);
    }
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vec4) * num_vertices, total_vertices);
    glBufferSubData(GL_ARRAY_BUFFER, bufferCounter, sizeof(vec4) * num_vertices, colors);
    
    GLuint vPosition = glGetAttribLocation(program, "vPosition");
    glEnableVertexAttribArray(vPosition);
    glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
    GLuint vColor = glGetAttribLocation(program, "vColor");
    glEnableVertexAttribArray(vColor);
    glVertexAttribPointer(vColor, 4, GL_FLOAT, GL_FALSE, 0, (GLvoid *) (sizeof(vec4)*num_vertices));
    
    translate_loc = glGetUniformLocation(program, "translate");
    scale_loc = glGetUniformLocation(program, "scale");
    rotation_loc = glGetUniformLocation(program, "rotation");
    
    glEnable(GL_DEPTH_TEST);
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glDepthRange(1,0);
}

void display(void) {
    mat4 temp0, temp1;
    glClearColor(0.0, 0, 0, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glPolygonMode(GL_FRONT, GL_FILL);
    glPolygonMode(GL_BACK, GL_FILL);
    float scaleFactor = .5;
    scalarMultMatrix(&identity, scaleFactor, &temp0);
    glUniformMatrix4fv(scale_loc, 1, GL_TRUE, &temp0);
    for(int i = 0; i < 4; i++) {
        if(i == 0) {
            getXRotationMatrixTheta(thetaX, &temp1);
            glUniformMatrix4fv(rotation_loc, 1, GL_TRUE, &temp1);
        } else if(i == 1) {
            getYRotationMatrixTheta(thetaX, &temp1);
            glUniformMatrix4fv(rotation_loc, 1, GL_TRUE, &temp1);
        } else if(i == 2) {
            getZRotationMatrix(thetaX, &temp1);
            glUniformMatrix4fv(rotation_loc, 1, GL_TRUE, &temp1);
        } else if(i == 3) {
            getXRotationMatrixTheta(thetaX*2, &temp1);
            glUniformMatrix4fv(rotation_loc, 1, GL_TRUE, &temp1);
        }
        glUniformMatrix4fv(translate_loc, 1, GL_TRUE, &objs[i].translation);
        glDrawArrays(GL_QUADS, objs[i].buffer_start_loc, objs[i].num_verts);
    }
    glutSwapBuffers();
    
}

void keyboard(unsigned char key, int mousex, int mousey)
{
    if(key == 'q')
    exit(0);
    
    glutPostRedisplay();
}

void mouseFunction(int button, int state, int x, int y) {
    
}

void dragFunction(int x, int y) {
    
}

void idle() {
    thetaX+=.01;
    thetaY+=.01;
    thetaZ+=.01;
    glutPostRedisplay();
}


int main(int argc, const char * argv[]) {
    getIdentityMatrix(&identity);
    srand(time(NULL));
    
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
    glutInitWindowSize(512, 512);
    glutInitWindowPosition(100,100);
    glutCreateWindow("Lab3");
    init();
    glutDisplayFunc(display);
    glutKeyboardFunc(keyboard);
    glutMouseFunc(mouseFunction);
    glutMotionFunc(dragFunction);
    glutIdleFunc(idle);
    glutMainLoop();
    
    return 0;
}
