﻿// FBO1.cpp : Sierpinski gasket with vertex arrays
//


#include "common.h"
#include <glm/vec2.hpp>

using namespace glm;

const int NumTimesToSubdivide = 5;
const int NumTriangles = 243;  // 3^5 triangles generated
const int NumVertices = 3 * NumTriangles;

const int WINSIZE = 512;
const int TEXSIZE = 256;

vec2 points[NumVertices];

// Two triangles for quad to draw texture in
// quad[6] extra point used to add random points dynamically
vec2 quad[7] = { 
    vec2(-1.0, -1.0), 
    vec2(-1.0, 1.0), 
    vec2(1.0, 1.0), 
    vec2(1.0, 1.0), 
    vec2(1.0, -1.0), 
    vec2(-1.0, -1.0), 
    vec2(0.0, 0.0) };

// texture coordinares for quad
vec2 tex[6] = { 
    vec2(0.0, 0.0), 
    vec2(0.0, 1.0), 
    vec2(1.0, 1.0), 
    vec2(1.0, 1.0), 
    vec2(1.0, 0.0), 
    vec2(0.0, 0.0) };

int Index = 0;

int texbuffer = 1;

GLuint program1, program2;
GLuint framebuffer, texture;
//GLuint renderbuffer;
GLuint buffer;
GLuint vao;

//----------------------------------------------------------------------------

void
triangle(const vec2& a, const vec2& b, const vec2& c)
{
    points[Index++] = a;
    points[Index++] = b;
    points[Index++] = c;
}

//----------------------------------------------------------------------------

void
divide_triangle(const vec2& a, const vec2& b, const vec2& c, int count)
{
    if (count > 0) {
        vec2 v0 = (a + b) / vec2(2.0);
        vec2 v1 = (a + c) / vec2(2.0);
        vec2 v2 = (b + c) / vec2(2.0);
        divide_triangle(a, v0, v1, count - 1);
        divide_triangle(c, v1, v2, count - 1);
        divide_triangle(b, v2, v0, count - 1);
    }
    else {
        triangle(a, b, c);    // draw triangle at end of recursion
    }
}

//----------------------------------------------------------------------------

void
init(void)
{
    glPointSize(10.0);

    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, TEXSIZE, TEXSIZE, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    GLuint renderbuffer;
    glGenRenderbuffers(1, &renderbuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, renderbuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, TEXSIZE, TEXSIZE);

    glGenFramebuffers(1, &framebuffer);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, framebuffer);
    glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);
    glFramebufferRenderbuffer(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, renderbuffer);
    int status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (status == GL_FRAMEBUFFER_COMPLETE) 
        printf("Frame Buffer Complete\n");

    glEnable(GL_DEPTH_TEST);

    // Specifiy the vertices for a triangle
    vec2 vertices[3] = {
        vec2(-1.0, -1.0),
        vec2(0.0, 1.0),
        vec2(1.0, -1.0)
    };

    // Subdivide the original triangle
    divide_triangle(vertices[0], vertices[1], vertices[2], NumTimesToSubdivide);

    // Create a vertex array object
    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    // Create and initialize a buffer object
    glGenBuffers(1, &buffer);
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(points), points, GL_STATIC_DRAW);

    // Load shaders and use the resulting shader program
    program1 = InitShader("vshader1.glsl", "fshader1.glsl");
    program2 = InitShader("vshader2.glsl", "fshader2.glsl");

    glClearColor(1.0, 1.0, 1.0, 1.0); /* white background */
}

//----------------------------------------------------------------------------

void
display(void)
{
    // render into render buffer

    glUseProgram(program1);

    // Initialize the vertex position attribute from the vertex shader    
    GLuint loc = glGetAttribLocation(program1, "vPosition");
    glEnableVertexAttribArray(loc);
    glVertexAttribPointer(loc, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

    glBindBuffer(GL_ARRAY_BUFFER, buffer);

    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, framebuffer);
    glViewport(0, 0, TEXSIZE, TEXSIZE);
    glClearColor(1.0, 1.0, 1.0, 1.0);

    // normal render
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glDrawArrays(GL_TRIANGLES, 0, 3 * NumTriangles);

    // Bind to window system frame buffer, unbind the texture
    glGenerateMipmap(GL_TEXTURE_2D);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    glEnable(GL_TEXTURE_2D);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);


    glBufferData(GL_ARRAY_BUFFER, sizeof(quad) + sizeof(tex), NULL, GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(quad), quad);
    glBufferSubData(GL_ARRAY_BUFFER, sizeof(quad), sizeof(tex), tex);

    glActiveTexture(GL_TEXTURE0);
    glUseProgram(program2);

    glDisableVertexAttribArray(loc);
    GLuint quad_loc = glGetAttribLocation(program2, "vPosition");
    glEnableVertexAttribArray(quad_loc);
    glVertexAttribPointer(quad_loc, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

    GLuint vTexCoord = glGetAttribLocation(program2, "vTexCoord");
    glEnableVertexAttribArray(vTexCoord);
    glVertexAttribPointer(vTexCoord, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(quad)));

    glUniform1i(glGetUniformLocation(program2, "texture"), 0);
    glUniform1f(glGetUniformLocation(program2, "d"), 1.0 / ((float)TEXSIZE));
    glBindTexture(GL_TEXTURE_2D, texture);

    glViewport(0, 0, WINSIZE, WINSIZE);
    glClearColor(0.0, 0.0, 0.0, 1.0);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glDrawArrays(GL_TRIANGLES, 0, 6);

    glutSwapBuffers();
}

//----------------------------------------------------------------------------

void
keyboard(unsigned char key, int x, int y)
{
    switch (key) {
    case 033: // ESC, Eacape Key
        exit(EXIT_SUCCESS);
        break;
    }
}

//----------------------------------------------------------------------------

int main(int argc, char** argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE);
    glutInitWindowSize(WINSIZE, WINSIZE);
    glutCreateWindow("Sierpinski Gasket (FBO)");

    glewInit();

    init();

    glutDisplayFunc(display);
    glutKeyboardFunc(keyboard);

    glutMainLoop();

    return EXIT_SUCCESS;
}