// NonPhoto.cpp : a simple example of non-photorealistic rendering using two colors and diffuse lighting. 
// 
// Non-photorealistic renderin = Cartoon shading 
// The example is a lot nicer if you replace the cube with a curved surface such as the teapot.


#include "common.h"
//#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace glm;

const int NumVertices = 36; //(6 faces)(2 triangles/face)(3 vertices/triangle)

vec4 points[NumVertices];
vec3 normals[NumVertices];

// Vertices of a unit cube centered at origin, sides aligned with axes
vec4 vertices[8] = {
    vec4(-0.5, -0.5,  0.5, 1.0),
    vec4(-0.5,  0.5,  0.5, 1.0),
    vec4( 0.5,  0.5,  0.5, 1.0),
    vec4( 0.5, -0.5,  0.5, 1.0),
    vec4(-0.5, -0.5, -0.5, 1.0),
    vec4(-0.5,  0.5, -0.5, 1.0),
    vec4( 0.5,  0.5, -0.5, 1.0),
    vec4( 0.5, -0.5, -0.5, 1.0)
};

// Array of rotation angles (in degrees) for each coordinate axis
enum { Xaxis = 0, Yaxis = 1, Zaxis = 2, NumAxes = 3 };
int     Axis = Xaxis;
GLfloat Theta[NumAxes] = { 0.0, 0.0, 0.0 };

// Model-view and projection matrices uniform location
GLuint  ModelView, Projection;

//----------------------------------------------------------------------------

// quad generates two triangles for each face and assigns colors
//    to the vertices

int Index = 0;

void
quad(int a, int b, int c, int d)
{
    // Initialize temporary vectors along the quad's edge to
    //   compute its face normal 
    vec4 u = vertices[b] - vertices[a];
    vec4 v = vertices[c] - vertices[b];
    vec3 normal = normalize(cross(vec3(u), vec3(v)));

    normals[Index] = normal; points[Index] = vertices[a]; Index++;
    normals[Index] = normal; points[Index] = vertices[b]; Index++;
    normals[Index] = normal; points[Index] = vertices[c]; Index++;
    normals[Index] = normal; points[Index] = vertices[a]; Index++;
    normals[Index] = normal; points[Index] = vertices[c]; Index++;
    normals[Index] = normal; points[Index] = vertices[d]; Index++;
}

//----------------------------------------------------------------------------

// generate 12 triangles: 36 vertices and 36 colors
void
colorcube()
{
    quad(1, 0, 3, 2);
    quad(2, 3, 7, 6);
    quad(3, 0, 4, 7);
    quad(6, 5, 1, 2);
    quad(4, 5, 6, 7);
    quad(5, 4, 0, 1);
}

//----------------------------------------------------------------------------

// OpenGL initialization
void
init()
{
    colorcube();

    // Create a vertex array object
    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    // Create and initialize a buffer object
    GLuint buffer;
    glGenBuffers(1, &buffer);
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(points) + sizeof(normals), NULL, GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(points), points);
    glBufferSubData(GL_ARRAY_BUFFER, sizeof(points), sizeof(normals), normals);

    // Load shaders and use the resulting shader program
    GLuint program = InitShader("vshader.glsl", "fshader.glsl");
    glUseProgram(program);

    // set up vertex arrays
    GLuint vPosition = glGetAttribLocation(program, "vPosition");
    glEnableVertexAttribArray(vPosition);
    glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

    GLuint vNormal = glGetAttribLocation(program, "vNormal");
    glEnableVertexAttribArray(vNormal);
    glVertexAttribPointer(vNormal, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(points)));

    // Initialize shader lighting parameters
    vec4 light_position(0.0, 0.0, -1.0, 0.0);
    vec4 light_ambient(0.2, 0.2, 0.2, 1.0);
    vec4 light_diffuse(1.0, 1.0, 1.0, 1.0);
    vec4 light_specular(1.0, 1.0, 1.0, 1.0);

    vec4 material_ambient(1.0, 0.0, 1.0, 1.0);
    vec4 material_diffuse(1.0, 0.8, 0.0, 1.0);
    vec4 material_specular(1.0, 0.8, 0.0, 1.0);
    float material_shininess = 100.0;

    vec4 ambient_product = light_ambient * material_ambient;
    vec4 diffuse_product = light_diffuse * material_diffuse;
    vec4 specular_product = light_specular * material_specular;

    glUniform4fv(glGetUniformLocation(program, "AmbientProduct"), 1, value_ptr(ambient_product));
    glUniform4fv(glGetUniformLocation(program, "DiffuseProduct"), 1, value_ptr(diffuse_product));
    glUniform4fv(glGetUniformLocation(program, "SpecularProduct"), 1, value_ptr(specular_product));

    glUniform4fv(glGetUniformLocation(program, "LightPosition"), 1, value_ptr(light_position));

    glUniform1f(glGetUniformLocation(program, "Shininess"), material_shininess);

    // Retrieve transformation uniform variable locations
    ModelView = glGetUniformLocation(program, "ModelView");
    Projection = glGetUniformLocation(program, "Projection");

    glEnable(GL_DEPTH_TEST);

    glShadeModel(GL_FLAT);

    glClearColor(1.0, 1.0, 1.0, 1.0);
}

//----------------------------------------------------------------------------

void
display(void)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    //  Generate tha model-view matrixn

    const vec3 viewer_pos(0.0, 0.0, 2.0);
    mat4 T = translate(mat4(1.0), -viewer_pos);
    mat4 Rx = rotate(mat4(1.0), DegreesToRadians * Theta[Xaxis], vec3(1, 0, 0));
    mat4 Ry = rotate(mat4(1.0), DegreesToRadians * Theta[Yaxis], vec3(0, 1, 0));
    mat4 Rz = rotate(mat4(1.0), DegreesToRadians * Theta[Zaxis], vec3(0, 0, 1));
    mat4  model_view = T * Rx * Ry * Rz;

    glUniformMatrix4fv(ModelView, 1, GL_FALSE, value_ptr(model_view));

    glDrawArrays(GL_TRIANGLES, 0, NumVertices);
    glutSwapBuffers();
}

//----------------------------------------------------------------------------

void
mouse(int button, int state, int x, int y)
{
    if (state == GLUT_DOWN) {
        switch (button) {
        case GLUT_LEFT_BUTTON:    Axis = Xaxis;  break;
        case GLUT_MIDDLE_BUTTON:  Axis = Yaxis;  break;
        case GLUT_RIGHT_BUTTON:   Axis = Zaxis;  break;
        }
    }
}

//----------------------------------------------------------------------------

void
idle(void)
{
    Theta[Axis] += 0.5;

    if (Theta[Axis] > 360.0) {
        Theta[Axis] -= 360.0;
    }

    glutPostRedisplay();
}

//----------------------------------------------------------------------------

void
keyboard(unsigned char key, int x, int y)
{
    switch (key) {
    case 033: // Escape Key
    case 'q': case 'Q':
        exit(EXIT_SUCCESS);
        break;
    }
}

//----------------------------------------------------------------------------

void
reshape(int width, int height)
{
    glViewport(0, 0, width, height);

    GLfloat aspect = GLfloat(width) / height;
    mat4  projection = perspective(45.0f, aspect, 0.5f, 3.0f);

    glUniformMatrix4fv(Projection, 1, GL_FALSE, value_ptr(projection));
}

//----------------------------------------------------------------------------

int
main(int argc, char** argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
    glutInitWindowSize(512, 512);
    glutInitContextVersion(3, 2);
    glutInitContextProfile(GLUT_CORE_PROFILE);
    glutCreateWindow("Color Cube");

    glewInit();

    init();

    glutDisplayFunc(display);
    glutKeyboardFunc(keyboard);
    glutReshapeFunc(reshape);
    glutMouseFunc(mouse);
    glutIdleFunc(idle);

    glutMainLoop();
    return 0;
}
