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
#include "genMaze.h"
#include "initShader.h"
#include "viewFuncs.h"
#include "shapeVecs.h"


#define BUFFER_OFFSET( offset )   ((GLvoid*) (offset))

mat4 identity;
vec4* colors;
vec4 total_vertices[20000];
obj objs[300];
int numObjs = 0;
int screenHeight = 512;
int screenWidth = 512;
int bufferCounter = 0;
int num_vertices;
GLuint translate_loc;
GLuint scale_loc;
GLuint rotation_loc;
GLuint mvm_loc;
GLuint proj_mat_loc;
float thetaX, thetaY, thetaZ;
mat4 mvm;

mat4 proj_mat;
float p_near = 4.42;
float p_far = -1.8;
float p_left = .3;
float p_right = .3;
float p_top = .3;
float p_bot = .3;

vec4 _eye, _at, _up;
float eyeTheta = 0;
float camHeight = 0;

int mazeSize = 8;
float cell_size = .1f;

cell *cellz;


void initObjs() {
    vec4 v0, v1, v2, v3;
    int i, j;
    i = 0;
    j = 0;
    for(i = 0; i < 4; i++ ) {
//        objs[i].vertices = malloc(sizeof(vec4) * 24);
        objs[i].num_verts = 24;
        for(j = 0; j < objs[i].num_verts; j++) {
            objs[i].vertices[j].x = cubeVertices[j].x;
            objs[i].vertices[j].y = cubeVertices[j].y;
            objs[i].vertices[j].z = cubeVertices[j].z;
            objs[i].vertices[j].w = cubeVertices[j].w;
        }
        num_vertices += 24;
        getTranslationMatrix(0, 0, 0, &objs[i].translation);
        objs[i].rotation = identity;
    }
}

void rectObj(float xScale, float yScale, float zScale, float x, float y, float z, obj *in) {
    
    in->vertices = (vec4 *)malloc(sizeof(vec4) * 24);
    in->num_verts = 24;
    for(int j = 0; j < in->num_verts; j++) {
        in->vertices[j].x = cubeVertices[j].x * xScale * 2;
        in->vertices[j].y = cubeVertices[j].y * yScale * 2;
        in->vertices[j].z = cubeVertices[j].z * zScale * 2;
        in->vertices[j].w = cubeVertices[j].w;
    }
    num_vertices += 24;
    getTranslationMatrix(x, y, z, &in->translation);
    in->rotation = identity;
}

void initMazeObjs() {
    
    int row, col;
    float mazeCenterX = 0; //(mazeSize/2)*cell_size;
    float mazeCenterZ = 0; //(mazeSize/2)*cell_size;
    defineVector(0, -.1, 0, 1, &_eye);
    defineVector(0, 0, 0, 1, &_at);
    defineVector(0, 0, 1, 1, &_up);
    look_at(_eye.x, _eye.y, _eye.z, _at.x, _at.y, _at.z, _up.x, _up.y, _up.z, &mvm);
    
    
    cell (*cells2D)[mazeSize] = (cell (*)[mazeSize]) cellz;
    
    // Set surrounding walls
    for(col = 0; col < mazeSize; col++) {
        for (row = 0; row < mazeSize; row++) {
            vec4 cell_loc;
            defineVector(-1*mazeCenterX + col*cell_size, 0, (-1)*mazeCenterZ + row*cell_size, 0, &cell_loc);
            
            if(cells2D[row][col].west == 1) {
                rectObj(cell_size/10, cell_size, cell_size, cell_loc.x-(cell_size/2.0f), cell_loc.y, cell_loc.z, &(objs[numObjs]));
                numObjs++;
            }
            
            if(col == mazeSize - 1) {
                if(cells2D[row][col].east == 1) {
                    rectObj(cell_size/10, cell_size, cell_size, cell_loc.x+(cell_size/2.0f), cell_loc.y, cell_loc.z, &(objs[numObjs]));
                    numObjs++;
                }
            }
            
            if(cells2D[row][col].north == 1) {
                rectObj(cell_size, cell_size, cell_size/10, cell_loc.x, cell_loc.y, cell_loc.z-(cell_size/2.0f), &(objs[numObjs]));
                numObjs++;
            }
            
            if(row == mazeSize - 1) {
                if(cells2D[row][col].south == 1) {
                    rectObj(cell_size, cell_size, cell_size/10, cell_loc.x, cell_loc.y, cell_loc.z+(cell_size/2.0f), &(objs[numObjs]));
                    numObjs++;
                }
            }
        }
    }
}

void initColors(int num, int numPoints) {
    colors = (vec4 *) malloc(sizeof(vec4) * num);
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

void loadObjectOnBuffer(obj *o, int offset){
    //    glBufferSubData(GL_ARRAY_BUFFER, offset, o.num_verts*sizeof(vec4), o.vertices);
    for(int i = 0; i < o->num_verts; i++ ) {
        total_vertices[i+offset] = o->vertices[i];
    }
    bufferCounter+=sizeof(vec4) * o->num_verts;
    o->buffer_start_loc = offset;
}

void init(void)
{
    thetaX = 0;
    thetaY = 0;
    thetaZ = 0;
    
//    initObjs();
    initMazeObjs();
    initColors(num_vertices, 4);
    
    GLuint program = initShader("vshader.glsl", "fshader.glsl");
    glUseProgram(program);
    GLuint vao;
    glGenVertexArraysAPPLE(1, &vao);
    glBindVertexArrayAPPLE(vao);
    GLuint buffer;
    glGenBuffers(1, &buffer);
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vec4)*num_vertices*2, NULL, GL_STATIC_DRAW);
    
    for(int i = 0; i < numObjs; i++ ){
        loadObjectOnBuffer(&(objs[i]), i*objs[i].num_verts);
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
    mvm_loc = glGetUniformLocation(program, "mvm");
    proj_mat_loc = glGetUniformLocation(program, "proj_mat");
    
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
    glUniformMatrix4fv(mvm_loc, 1, GL_TRUE, &mvm);
    glUniformMatrix4fv(proj_mat_loc, 1, GL_TRUE, &proj_mat);

    for(int i = 0; i < numObjs; i++) {
//        if(i == 0) {
//            getXRotationMatrixTheta(thetaX, &temp1);
//            glUniformMatrix4fv(rotation_loc, 1, GL_TRUE, &temp1);
//        } else if(i == 1) {
//            getYRotationMatrixTheta(thetaX, &temp1);
//            glUniformMatrix4fv(rotation_loc, 1, GL_TRUE, &temp1);
//        } else if(i == 2) {
//            getZRotationMatrix(thetaX, &temp1);
//            glUniformMatrix4fv(rotation_loc, 1, GL_TRUE, &temp1);
//        } else if(i == 3) {
//            getXRotationMatrixTheta(thetaX*2, &temp1);
//            glUniformMatrix4fv(rotation_loc, 1, GL_TRUE, &temp1);
//        } else {
        glUniformMatrix4fv(rotation_loc, 1, GL_TRUE, &identity);
//        }
        glUniformMatrix4fv(translate_loc, 1, GL_TRUE, &objs[i].translation);
        glDrawArrays(GL_QUADS, objs[i].buffer_start_loc, objs[i].num_verts);
    }
    glutSwapBuffers();
    
}

void keyboard(unsigned char key, int mousex, int mousey) {
    if(key == 'i') {
        p_near+=.02;
//        frustum(p_left, p_right, p_bot, p_top, p_near, p_far, &proj_mat);
        perspective(p_right, p_top, p_near, p_far, &proj_mat);
    }
    if(key == 'k') {
        p_near-=.02;
//        frustum(p_left, p_right, p_bot, p_top, p_near, p_far, &proj_mat);
        perspective(p_right, p_top, p_near, p_far, &proj_mat);
    }
    if(key == 'o') {
        p_far+=.02;
//        frustum(p_left, p_right, p_bot, p_top, p_near, p_far, &proj_mat);
        perspective(p_right, p_top, p_near, p_far, &proj_mat);
    }
    if(key == 'l') {
        p_far-=.02;
//        frustum(p_left, p_right, p_bot, p_top, p_near, p_far, &proj_mat);
        perspective(p_right, p_top, p_near, p_far, &proj_mat);
    }
    if(key == 'p') {
        p_right+=.02;
//        frustum(p_left, p_right, p_bot, p_top, p_near, p_far, &proj_mat);
        perspective(p_right, p_top, p_near, p_far, &proj_mat);
    }
    if(key == ';') {
        p_right-=.02;
//        frustum(p_left, p_right, p_bot, p_top, p_near, p_far, &proj_mat);
        perspective(p_right, p_top, p_near, p_far, &proj_mat);
    }
    if(key == 'r') {
        p_left = -1.0;
        p_right = 1.0;
        p_bot = -1.0;
        p_top = 1.0;
        p_near = 1.0;
        p_far = -1.0;
//        frustum(p_left, p_right, p_bot, p_top, p_near, p_far, &proj_mat);
        perspective(p_right, p_top, p_near, p_far, &proj_mat);
    }
    printf("left: %f, right: %f, bot: %f, top: %f, near: %f, far: %f\n", p_left, p_right, p_bot, p_top, p_near, p_far);
    
    
    // MVM keys
    if(key =='d') {
        _at.x+=.01;
        look_at(_eye.x, _eye.y, _eye.z, _at.x, _at.y, _at.z, _up.x, _up.y, _up.z, &mvm);
    }
    if(key =='a') {
        _at.x-=.01;
//        look_at_theta(eyeTheta, camHeight, &mvm);
        look_at(_eye.x, _eye.y, _eye.z, _at.x, _at.y, _at.z, _up.x, _up.y, _up.z, &mvm);
    }
    if(key =='w') {
        _at.z+=.01;
//        look_at_theta(eyeTheta, camHeight, &mvm);
        look_at(_eye.x, _eye.y, _eye.z, _at.x, _at.y, _at.z, _up.x, _up.y, _up.z, &mvm);
    }
    if(key =='s') {
        _at.z-=.01;
//        look_at_theta(eyeTheta, camHeight, &mvm);
        look_at(_eye.x, _eye.y, _eye.z, _at.x, _at.y, _at.z, _up.x, _up.y, _up.z, &mvm);
    }
    
    if(key == 'q')
        exit(0);
    glutPostRedisplay();
}

void mouseFunction(int button, int state, int x, int y) {
    
}

void dragFunction(int x, int y) {
    
}

void idle() {

    glutPostRedisplay();
}


int main(int argc, const char * argv[]) {
    
    
    getIdentityMatrix(&identity);
    multiplyMatrices(&identity, &identity, &mvm);
    multiplyMatrices(&identity, &identity, &proj_mat);
    srand(time(NULL));
    
    
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
    glutInitWindowSize(512, 512);
    glutInitWindowPosition(100,100);
    glutCreateWindow("Proj2");
    cellz = (cell *) malloc (mazeSize * mazeSize * sizeof(cell));
    gen_maze(8, 8, cellz);
    
    print_maze(mazeSize, mazeSize, cellz);
    
    init();
    glutDisplayFunc(display);
    glutKeyboardFunc(keyboard);
    glutMouseFunc(mouseFunction);
    glutMotionFunc(dragFunction);
    glutIdleFunc(idle);
    glutMainLoop();
    
    return 0;
}
