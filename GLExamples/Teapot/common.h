#pragma once
#pragma warning(disable:4996)

#include <cmath>
#include <iostream>

#include <GL/glew.h>
//#include <GL/glut.h>
#include <GL/freeglut.h>

// Define a helpful macro for handling offsets into buffer objects
#define BUFFER_OFFSET( offset )   ((GLvoid*) (offset))

//  Helper function to load vertex and fragment shader files
GLuint InitShader(const char* vShaderFile, const char* fShaderFile);

//  Define M_PI in the case it's not defined in the math header file
#ifndef M_PI
#  define M_PI  3.14159265358979323846
#endif

//  Degrees-to-radians constant 
const GLfloat  DegreesToRadians = M_PI / 180.0;



