// Teapot_original.cpp : 
//

#include <stdlib.h>
#include "common.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace glm;

#include "vertices.h"
#include "patches.h"

const int NumTriangles = (NumTeapotPatches * 2);
const int NumVertices = (NumTriangles * 3);

vec4    points[NumVertices];

GLuint  Projection;

enum { X = 0, Y = 1, Z = 2 };


//----------------------------------------------------------------------------

void
init(void)
{
    int Index = 0;
    for (int n = 0; n < NumTeapotPatches; n++) {
        vec4  patch[4][4];

        // Initialize each patch's control point data
        for (int i = 0; i < 4; ++i) {
            for (int j = 0; j < 4; ++j) {
                vec3& v = vertices[indices[n][i][j]];
                patch[i][j] = vec4(v[X], v[Y], v[Z], 1.0);
            }
        }

        // Draw the quad (as two triangles) bounded by the corners of the
        //   Bezier patch.
        points[Index++] = patch[0][0];
        points[Index++] = patch[3][0];
        points[Index++] = patch[3][3];
        points[Index++] = patch[0][0];
        points[Index++] = patch[3][3];
        points[Index++] = patch[0][3];
    }

    // Create a vertex array object
    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    // Create and initialize a buffer object
    GLuint buffer;
    glGenBuffers(1, &buffer);
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vec4) * NumVertices, points, GL_STATIC_DRAW);

    // Load shaders and use the resulting shader program
    GLuint program = InitShader("vshader.glsl", "fshader.glsl");
    glUseProgram(program);

    // set up vertex arrays
    GLuint vPosition = glGetAttribLocation(program, "vPosition");
    glEnableVertexAttribArray(vPosition);
    glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

    Projection = glGetUniformLocation(program, "Projection");

    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    glClearColor(1.0, 1.0, 1.0, 1.0);
}

//----------------------------------------------------------------------------

void
display(void)
{
    glClear(GL_COLOR_BUFFER_BIT);
    glDrawArrays(GL_TRIANGLES, 0, NumVertices);
    glutSwapBuffers();
}

//----------------------------------------------------------------------------

void
reshape(int width, int height)
{
    glViewport(0, 0, width, height);

    GLfloat  left = -4.0, right = 4.0;
    GLfloat  bottom = -3.0, top = 5.0;
    GLfloat  zNear = -10.0, zFar = 10.0;

    GLfloat  aspect = GLfloat(width) / height;

    if (aspect > 0) {
        left *= aspect;
        right *= aspect;
    }
    else {
        bottom /= aspect;
        top /= aspect;
    }

    mat4 projection = ortho(left, right, bottom, top, zNear, zFar);
    glUniformMatrix4fv(Projection, 1, GL_FALSE, value_ptr(projection));
}

//----------------------------------------------------------------------------

void
keyboard(unsigned char key, int x, int y)
{
    switch (key) {
    case 'q': case 'Q': case 27 /* Escape key */:
        exit(EXIT_SUCCESS);
        break;
    }
}


//----------------------------------------------------------------------------

int
main(int argc, char* argv[])
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE);
    glutInitWindowSize(512, 512);
    glutInitContextVersion(3, 2);
    glutInitContextProfile(GLUT_CORE_PROFILE);
    glutCreateWindow("Utah Teapot");

    glewInit();

    init();

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);

    glutMainLoop();

    free(points);
    return 0;
}
