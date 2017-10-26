//
//  types.h
//  Lab3
//
//  Created by Sam Nosenzo on 10/20/17.
//  Copyright © 2017 Sam Nosenzo. All rights reserved.
//

#ifndef types_h
#define types_h


#endif /* types_h */


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

typedef struct obj {
    vec4 *vertices;
    vec4 *colors;
    int num_verts;
    int buffer_start_loc;
    mat4 translation;
    mat4 rotation;
} obj;
