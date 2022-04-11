// Wave.cpp : a simple example of animating a rectangular mesh in the vertex shader.
// 
// The shading is not quite right since the normals aren't preturbed.

#include "common.h"
//#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace glm;

#define N 64
GLfloat data[N][N]; /* array of data heights */
vec4 points[6 * N * N];

const GLdouble nearVal = 1.0; /* near distance */
const GLdouble farVal = 20.0; /* far distance */
GLuint  program = 0; /* program object id */
GLint   timeParam;
GLuint  MVP_loc, MV_loc, N_loc;

//----------------------------------------------------------------------------

int Index = 0;
void mesh()
{
    /* flat mesh */
    int i, j;
    for (i = 0; i < N; i++)
        for (j = 0; j < N; j++)
            data[i][j] = 0.0;

    for (i = 0; i < N; i++) 
        for (j = 0; j < N; j++)
        {
            points[Index] = vec4((float)i / N,          data[i][j], (float)j / N,       1.0); Index++;
            points[Index] = vec4((float)i / N,          data[i][j], (float)(j + 1) / N, 1.0); Index++;
            points[Index] = vec4((float)(i + 1) / N,    data[i][j], (float)(j + 1) / N, 1.0); Index++;
            points[Index] = vec4((float)(i + 1) / N,    data[i][j], (float)(j + 1) / N, 1.0); Index++;
            points[Index] = vec4((float)(i + 1) / N,    data[i][j], (float)j / N,       1.0); Index++;
            points[Index] = vec4((float)i / N,          data[i][j], (float)j / N,       1.0); Index++;
        }
}

//----------------------------------------------------------------------------

void
init()
{
    mesh();

    // Create a vertex array object
    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    // Create and initialize a buffer object
    GLuint buffer;
    glGenBuffers(1, &buffer);
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(points), points, GL_STATIC_DRAW);

    // Load shaders and use the resulting shader program
    GLuint program = InitShader("vshader.glsl", "fshader.glsl");
    glUseProgram(program);

    // set up vertex arrays
    GLuint vPosition = glGetAttribLocation(program, "vPosition");
    glEnableVertexAttribArray(vPosition);
    glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

    timeParam = glGetUniformLocation(program, "time");
    MVP_loc = glGetUniformLocation(program, "ModelViewProjection");
    MV_loc = glGetUniformLocation(program, "ModelViewMatrix");

    glClearColor(1.0, 1.0, 1.0, 1.0);
    //glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
}

//----------------------------------------------------------------------------

void 
display(void)
{
    /* send elapsed time to shaders */

    mat4 projection = ortho(-0.75, 0.75, -0.75, 0.75, -5.5, 5.5);
    mat4 MV = lookAt(vec3(2.0, 2.0, 2.0), vec3(0.5, 0.0, 0.5), vec3(0.0, 1.0, 0.0));
    mat4 MVP = projection * MV;
    glUniformMatrix4fv(MV_loc, 1, GL_FALSE, value_ptr(MV));
    glUniformMatrix4fv(MVP_loc, 1, GL_FALSE, value_ptr(MVP));
    glUniform1f(timeParam, glutGet(GLUT_ELAPSED_TIME));

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glDrawArrays(GL_TRIANGLES, 0, 6 * N * N);
    
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
    glUniform1f(timeParam, (GLfloat)glutGet(GLUT_ELAPSED_TIME));
    glutPostRedisplay();
}

//----------------------------------------------------------------------------

int 
main(int argc, char** argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE);
    glutInitWindowSize(512, 512);
    //glutInitWindowSize(1280, 720);
    glutCreateWindow("Waving mesh example");

    glewInit();
    
    init();

    glutDisplayFunc(display);
    glutKeyboardFunc(keyboard);
    glutIdleFunc(idle);

    glutMainLoop();

    return EXIT_SUCCESS;
}
