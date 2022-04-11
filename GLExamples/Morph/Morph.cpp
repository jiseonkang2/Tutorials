// Morph.cpp : a demo of one triangle morphing into another
// 
// Two sets of vertex locations are sent to the shader. 
// The shader interpolates between pairs of corresponding vertices 
//    using a sinusoidally varying time parameter.

#include "common.h"
//#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace glm;

GLint timeParam;

//----------------------------------------------------------------------------

void init()
{
    vec2 vertices_one[3] = { 
        vec2(-1.0, -1.0), 
        vec2( 0.0,  1.0), 
        vec2( 1.0, -1.0) };
    vec2 vertices_two[3] = { 
        vec2(1.0, -1.0), 
        vec2(0.0, -1.0), 
        vec2(1.0,  1.0) };

    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    // Create and initialize a buffer object
    GLuint buffer;
    glGenBuffers(1, &buffer);
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices_one) + sizeof(vertices_two), NULL, GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices_one), vertices_one);
    glBufferSubData(GL_ARRAY_BUFFER, sizeof(vertices_one), sizeof(vertices_two), vertices_two);

    // Load shaders and use the resulting shader program
    GLuint program = InitShader("vshader.glsl", "fshader.glsl");
    glUseProgram(program);

    // Initialize the vertex position attribute from the vertex shader
    GLuint vertices_one_location = glGetAttribLocation(program, "vertices1");
    glEnableVertexAttribArray(vertices_one_location);
    glVertexAttribPointer(vertices_one_location, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

    GLuint vertices_two_location = glGetAttribLocation(program, "vertices2");
    glEnableVertexAttribArray(vertices_two_location);
    glVertexAttribPointer(vertices_two_location, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(vertices_one)));
    
    glClearColor(1.0, 1.0, 1.0, 1.0); /* white background */
}

//----------------------------------------------------------------------------

void 
display(void)
{
    /* send elapsed time to shaders */

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUniform1f(timeParam, glutGet(GLUT_ELAPSED_TIME));

    glDrawArrays(GL_LINE_LOOP, 0, 3);

    glutSwapBuffers();
}

//----------------------------------------------------------------------------

void 
keyboard(unsigned char key, int x, int y)
{
    switch (key) {
    case 27:
    case 'Q':
    case 'q':
        exit(EXIT_SUCCESS);
        break;
    default:
        break;
    }
}

//----------------------------------------------------------------------------

void 
idle()
{
    glUniform1f(timeParam, glutGet(GLUT_ELAPSED_TIME));
    glutPostRedisplay();
}

//----------------------------------------------------------------------------

void
reshape(int width, int height)
{
    glViewport(280, 0, 720, 720);
}

//----------------------------------------------------------------------------

#define HD_VIEW 0

int
main(int argc, char** argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE);
#if HD_VIEW
    glutInitWindowSize(1280, 720);
#else
    glutInitWindowSize(512, 512);
#endif


    glutCreateWindow("Mophing");

    glewInit();

    init();

    glutDisplayFunc(display);
    glutKeyboardFunc(keyboard);
    glutIdleFunc(idle);
#if HD_VIEW
    glutReshapeFunc(reshape);
#endif

    glutMainLoop();

    return EXIT_SUCCESS;
}
