// Robot.cpp : 
//

#include "common.h"
//#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <assert.h>

using namespace glm;

const int NumVertices = 36; //(6 faces)(2 triangles/face)(3 vertices/triangle)

vec4 points[NumVertices];
vec4 colors[NumVertices];

vec4 vertices[8] = {
    vec4(-0.5, -0.5, 0.5, 1.0),
    vec4(-0.5, 0.5, 0.5, 1.0),
    vec4( 0.5, 0.5, 0.5, 1.0),
    vec4( 0.5, -0.5, 0.5, 1.0),
    vec4(-0.5, -0.5, -0.5, 1.0),
    vec4(-0.5, 0.5, -0.5, 1.0),
    vec4( 0.5, 0.5, -0.5, 1.0),
    vec4( 0.5, -0.5, -0.5, 1.0)
};

// RGBA olors
vec4 vertex_colors[8] = {
    vec4(0.0, 0.0, 0.0, 1.0),  // black
    vec4(1.0, 0.0, 0.0, 1.0),  // red
    vec4(1.0, 1.0, 0.0, 1.0),  // yellow
    vec4(0.0, 1.0, 0.0, 1.0),  // green
    vec4(0.0, 0.0, 1.0, 1.0),  // blue
    vec4(1.0, 0.0, 1.0, 1.0),  // magenta
    vec4(1.0, 1.0, 1.0, 1.0),  // white
    vec4(0.0, 1.0, 1.0, 1.0)   // cyan
};

//----------------------------------------------------------------------------

class MatrixStack {
    int    _index;
    int    _size;
    mat4* _matrices;

public:
    MatrixStack(int numMatrices = 32) :_index(0), _size(numMatrices)
    {
        _matrices = new mat4[numMatrices];
    }

    ~MatrixStack()
    {
        delete[]_matrices;
    }

    void push(const mat4& m) {
        assert(_index + 1 < _size);
        _matrices[_index++] = m;
    }

    mat4& pop(void) {
        assert(_index - 1 >= 0);
        _index--;
        return _matrices[_index];
    }
};

MatrixStack  mvstack;
mat4         model_view;
GLuint       ModelView, Projection;

//----------------------------------------------------------------------------

#define TORSO_HEIGHT 5.0
#define TORSO_WIDTH 1.0
#define UPPER_ARM_HEIGHT 3.0
#define LOWER_ARM_HEIGHT 2.0
#define UPPER_LEG_WIDTH  0.5
#define LOWER_LEG_WIDTH  0.5
#define LOWER_LEG_HEIGHT 2.0
#define UPPER_LEG_HEIGHT 3.0
#define UPPER_LEG_WIDTH  0.5
#define UPPER_ARM_WIDTH  0.5
#define LOWER_ARM_WIDTH  0.5
#define HEAD_HEIGHT 1.5
#define HEAD_WIDTH 1.0

// Set up menu item indices, which we can alos use with the joint angles
enum {
    Torso = 0,
    Head = 1,
    Head1 = 1,
    Head2 = 2,
    LeftUpperArm = 3,
    LeftLowerArm = 4,
    RightUpperArm = 5,
    RightLowerArm = 6,
    LeftUpperLeg = 7,
    LeftLowerLeg = 8,
    RightUpperLeg = 9,
    RightLowerLeg = 10,
    NumNodes,
    Quit
};

// Joint angles with initial values
GLfloat
theta[NumNodes] = {
    0.0,    // Torso
    0.0,    // Head1
    0.0,    // Head2
    0.0,    // LeftUpperArm
    0.0,    // LeftLowerArm
    0.0,    // RightUpperArm
    0.0,    // RightLowerArm
    180.0,  // LeftUpperLeg
    0.0,     // LeftLowerLeg
    180.0,  // RightUpperLeg
    0.0    // RightLowerLeg
};

GLuint joint_angle = Head2;

//----------------------------------------------------------------------------

struct Node {
    mat4  transform;
    void  (*render)(void);
    Node* sibling;
    Node* child;

    Node() :
        render(NULL), sibling(NULL), child(NULL) {}

    Node(mat4& m, void (*render)(void), Node* sibling, Node* child) :
        transform(m), render(render), sibling(sibling), child(child) {}
};

Node  nodes[NumNodes];

//----------------------------------------------------------------------------

int Index = 0;

void
quad(int a, int b, int c, int d)
{
    colors[Index] = vertex_colors[a]; points[Index] = vertices[a]; Index++;
    colors[Index] = vertex_colors[a]; points[Index] = vertices[b]; Index++;
    colors[Index] = vertex_colors[a]; points[Index] = vertices[c]; Index++;
    colors[Index] = vertex_colors[a]; points[Index] = vertices[a]; Index++;
    colors[Index] = vertex_colors[a]; points[Index] = vertices[c]; Index++;
    colors[Index] = vertex_colors[a]; points[Index] = vertices[d]; Index++;
}

void
colorcube(void)
{
    quad(1, 0, 3, 2);
    quad(2, 3, 7, 6);
    quad(3, 0, 4, 7);
    quad(6, 5, 1, 2);
    quad(4, 5, 6, 7);
    quad(5, 4, 0, 1);
}

//----------------------------------------------------------------------------

void
traverse(Node* node)
{
    if (node == NULL) { return; }

    mvstack.push(model_view);

    model_view *= node->transform;
    node->render();

    if (node->child) { traverse(node->child); }

    model_view = mvstack.pop();

    if (node->sibling) { traverse(node->sibling); }
}

//----------------------------------------------------------------------------

void
torso()
{
    mvstack.push(model_view);

    mat4 T = translate(mat4(1.0), vec3(0.0, 0.5 * TORSO_HEIGHT, 0.0));
    mat4 S = scale(mat4(1.0), vec3(TORSO_WIDTH, TORSO_HEIGHT, TORSO_WIDTH));
    mat4 instance = T * S;

    glUniformMatrix4fv(ModelView, 1, GL_FALSE, value_ptr(model_view * instance));
    glDrawArrays(GL_TRIANGLES, 0, NumVertices);

    model_view = mvstack.pop();
}

void
head()
{
    mvstack.push(model_view);

    mat4 T = translate(mat4(1.0), vec3(0.0, 0.5 * HEAD_HEIGHT, 0.0));
    mat4 S = scale(mat4(1.0), vec3(HEAD_WIDTH, HEAD_HEIGHT, HEAD_WIDTH));
    mat4 instance = T * S;

    glUniformMatrix4fv(ModelView, 1, GL_FALSE, value_ptr(model_view * instance));
    glDrawArrays(GL_TRIANGLES, 0, NumVertices);

    model_view = mvstack.pop();
}

void
left_upper_arm()
{
    mvstack.push(model_view);

    mat4 T = translate(mat4(1.0), vec3(0.0, 0.5 * UPPER_ARM_HEIGHT, 0.0));
    mat4 S = scale(mat4(1.0), vec3(UPPER_ARM_WIDTH, UPPER_ARM_HEIGHT, UPPER_ARM_WIDTH));
    mat4 instance = T * S;

    glUniformMatrix4fv(ModelView, 1, GL_FALSE, value_ptr(model_view * instance));
    glDrawArrays(GL_TRIANGLES, 0, NumVertices);

    model_view = mvstack.pop();
}

void
left_lower_arm()
{
    mvstack.push(model_view);

    mat4 T = translate(mat4(1.0), vec3(0.0, 0.5 * LOWER_ARM_HEIGHT, 0.0));
    mat4 S = scale(mat4(1.0), vec3(LOWER_ARM_WIDTH, LOWER_ARM_HEIGHT, LOWER_ARM_WIDTH));
    mat4 instance = T * S;

    glUniformMatrix4fv(ModelView, 1, GL_FALSE, value_ptr(model_view * instance));
    glDrawArrays(GL_TRIANGLES, 0, NumVertices);

    model_view = mvstack.pop();
}

void
right_upper_arm()
{
    mvstack.push(model_view);

    mat4 T = translate(mat4(1.0), vec3(0.0, 0.5 * UPPER_ARM_HEIGHT, 0.0));
    mat4 S = scale(mat4(1.0), vec3(UPPER_ARM_WIDTH, UPPER_ARM_HEIGHT, UPPER_ARM_WIDTH));
    mat4 instance = T * S;

    glUniformMatrix4fv(ModelView, 1, GL_FALSE, value_ptr(model_view * instance));
    glDrawArrays(GL_TRIANGLES, 0, NumVertices);

    model_view = mvstack.pop();
}

void
right_lower_arm()
{
    mvstack.push(model_view);

    mat4 T = translate(mat4(1.0), vec3(0.0, 0.5 * LOWER_ARM_HEIGHT, 0.0));
    mat4 S = scale(mat4(1.0), vec3(LOWER_ARM_WIDTH, LOWER_ARM_HEIGHT, LOWER_ARM_WIDTH));
    mat4 instance = T * S;

    glUniformMatrix4fv(ModelView, 1, GL_FALSE, value_ptr(model_view * instance));
    glDrawArrays(GL_TRIANGLES, 0, NumVertices);

    model_view = mvstack.pop();
}

void
left_upper_leg()
{
    mvstack.push(model_view);

    mat4 T = translate(mat4(1.0), vec3(0.0, 0.5 * UPPER_LEG_HEIGHT, 0.0));
    mat4 S = scale(mat4(1.0), vec3(UPPER_LEG_WIDTH, UPPER_LEG_HEIGHT, UPPER_LEG_WIDTH));
    mat4 instance = T * S;

    glUniformMatrix4fv(ModelView, 1, GL_FALSE, value_ptr(model_view * instance));
    glDrawArrays(GL_TRIANGLES, 0, NumVertices);

    model_view = mvstack.pop();
}

void
left_lower_leg()
{
    mvstack.push(model_view);

    mat4 T = translate(mat4(1.0), vec3(0.0, 0.5 * LOWER_LEG_HEIGHT, 0.0));
    mat4 S = scale(mat4(1.0), vec3(LOWER_LEG_WIDTH, LOWER_LEG_HEIGHT, LOWER_LEG_WIDTH));
    mat4 instance = T * S;

    glUniformMatrix4fv(ModelView, 1, GL_FALSE, value_ptr(model_view * instance));
    glDrawArrays(GL_TRIANGLES, 0, NumVertices);

    model_view = mvstack.pop();
}

void
right_upper_leg()
{
    mvstack.push(model_view);

    mat4 T = translate(mat4(1.0), vec3(0.0, 0.5 * UPPER_LEG_HEIGHT, 0.0));
    mat4 S = scale(mat4(1.0), vec3(UPPER_LEG_WIDTH, UPPER_LEG_HEIGHT, UPPER_LEG_WIDTH));
    mat4 instance = T * S;

    glUniformMatrix4fv(ModelView, 1, GL_FALSE, value_ptr(model_view * instance));
    glDrawArrays(GL_TRIANGLES, 0, NumVertices);

    model_view = mvstack.pop();
}

void
right_lower_leg()
{
    mvstack.push(model_view);

    mat4 T = translate(mat4(1.0), vec3(0.0, 0.5 * LOWER_LEG_HEIGHT, 0.0));
    mat4 S = scale(mat4(1.0), vec3(LOWER_LEG_WIDTH, LOWER_LEG_HEIGHT, LOWER_LEG_WIDTH));
    mat4 instance = T * S;

    glUniformMatrix4fv(ModelView, 1, GL_FALSE, value_ptr(model_view * instance));
    glDrawArrays(GL_TRIANGLES, 0, NumVertices);

    model_view = mvstack.pop();
}

//----------------------------------------------------------------------------

void
display()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    traverse(&nodes[Torso]);
    glutSwapBuffers();
}

//----------------------------------------------------------------------------

void
mouse(int button, int state, int x, int y)
{
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
        theta[joint_angle] += 5.0;
        if (theta[joint_angle] > 360.0) { theta[joint_angle] -= 360.0; }
    }

    if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN) {
        theta[joint_angle] -= 5.0;
        if (theta[joint_angle] < 0.0) { theta[joint_angle] += 360.0; }
    }

    mvstack.push(model_view);

    mat4  T, T1, T2, Rx, Ry;

    switch (joint_angle) {
    case Torso:
        Ry = rotate(mat4(1.0), DegreesToRadians * theta[Torso], vec3(0, 1, 0));
        nodes[Torso].transform = Ry;
        break;

    case Head1: case Head2:
        T1 = translate(mat4(1.0), vec3(0.0, TORSO_HEIGHT + 0.5 * HEAD_HEIGHT, 0.0));
        Rx = rotate(mat4(1.0), DegreesToRadians * theta[Head1], vec3(1, 0, 0));
        Ry = rotate(mat4(1.0), DegreesToRadians * theta[Head2], vec3(0, 1, 0));
        T2 = translate(mat4(1.0), vec3(0.0, -0.5 * HEAD_HEIGHT, 0.0));
        nodes[Head].transform = T1 * Rx * Ry * T2;
        break;

    case LeftUpperArm:
        T = translate(mat4(1.0), vec3(-(TORSO_WIDTH + UPPER_ARM_WIDTH), 0.9 * TORSO_HEIGHT, 0.0));
        Rx = rotate(mat4(1.0), DegreesToRadians * theta[LeftUpperArm], vec3(1, 0, 0));
        nodes[LeftUpperArm].transform = T * Rx;
        break;

    case RightUpperArm:
        T = translate(mat4(1.0), vec3(TORSO_WIDTH + UPPER_ARM_WIDTH, 0.9 * TORSO_HEIGHT, 0.0));
        Rx = rotate(mat4(1.0), DegreesToRadians * theta[RightUpperArm], vec3(1, 0, 0));
        nodes[RightUpperArm].transform = T * Rx;
        break;

    case RightUpperLeg:
        T = translate(mat4(1.0), vec3(TORSO_WIDTH + UPPER_LEG_WIDTH, 0.1 * UPPER_LEG_HEIGHT, 0.0));
        Rx = rotate(mat4(1.0), DegreesToRadians * theta[RightUpperLeg], vec3(1, 0, 0));
        nodes[RightUpperLeg].transform = T * Rx;
        break;

    case LeftUpperLeg:
        T = translate(mat4(1.0), vec3(-(TORSO_WIDTH + UPPER_LEG_WIDTH), 0.1 * UPPER_LEG_HEIGHT, 0.0));
        Rx = rotate(mat4(1.0), DegreesToRadians * theta[LeftUpperLeg], vec3(1, 0, 0));
        nodes[LeftUpperLeg].transform = T * Rx;
        break;

    case LeftLowerArm:
        T = translate(mat4(1.0), vec3(0.0, UPPER_ARM_HEIGHT, 0.0));
        Rx = rotate(mat4(1.0), DegreesToRadians * theta[LeftLowerArm], vec3(1, 0, 0));
        nodes[LeftLowerArm].transform = T * Rx;
        break;

    case LeftLowerLeg:
        T = translate(mat4(1.0), vec3(0.0, UPPER_LEG_HEIGHT, 0.0));
        Rx = rotate(mat4(1.0), DegreesToRadians * theta[LeftLowerLeg], vec3(1, 0, 0));
        nodes[LeftLowerLeg].transform = T * Rx;
        break;

    case RightLowerLeg:
        T = translate(mat4(1.0), vec3(0.0, UPPER_LEG_HEIGHT, 0.0));
        Rx = rotate(mat4(1.0), DegreesToRadians * theta[RightLowerLeg], vec3(1, 0, 0));
        nodes[RightLowerLeg].transform = T * Rx;
        break;

    case RightLowerArm:
        T = translate(mat4(1.0), vec3(0.0, UPPER_ARM_HEIGHT, 0.0));
        Rx = rotate(mat4(1.0), DegreesToRadians * theta[RightLowerArm], vec3(1, 0, 0));
        nodes[RightLowerArm].transform = T * Rx;
        break;
    }

    model_view = mvstack.pop();
    glutPostRedisplay();
}

//----------------------------------------------------------------------------

void
menu(int option)
{
    if (option == Quit) {
        exit(EXIT_SUCCESS);
    }

    joint_angle = option;
}

//----------------------------------------------------------------------------

void
reshape(int width, int height)
{
    glViewport(0, 0, width, height);

    GLfloat left = -10.0, right = 10.0;
    GLfloat bottom = -10.0, top = 10.0;
    GLfloat zNear = -10.0, zFar = 10.0;

    GLfloat aspect = GLfloat(width) / height;

    if (aspect > 1.0) {
        left *= aspect;
        right *= aspect;
    }
    else {
        bottom /= aspect;
        top /= aspect;
    }

    mat4 projection = ortho(left, right, bottom, top, zNear, zFar);
    glUniformMatrix4fv(Projection, 1, GL_FALSE, value_ptr(projection));

    model_view = mat4(1.0);   // An Identity matrix
}

//----------------------------------------------------------------------------

void
initNodes(void)
{
    mat4  m, T, Rx, Ry;

    m = rotate(mat4(1.0), DegreesToRadians * theta[Torso], vec3(0, 1, 0));
    nodes[Torso] = Node(m, torso, NULL, &nodes[Head1]);

    T = translate(mat4(1.0), vec3(0.0, TORSO_HEIGHT + 0.5 * HEAD_HEIGHT, 0.0));
    Rx = rotate(mat4(1.0), DegreesToRadians * theta[Head1], vec3(1, 0, 0));
    Ry = rotate(mat4(1.0), DegreesToRadians * theta[Head2], vec3(0, 1, 0));
    m = T * Rx * Ry;
    nodes[Head1] = Node(m, head, &nodes[LeftUpperArm], NULL);

    T = translate(mat4(1.0), vec3(-(TORSO_WIDTH + UPPER_ARM_WIDTH), 0.9 * TORSO_HEIGHT, 0.0));
    Rx = rotate(mat4(1.0), DegreesToRadians * theta[LeftUpperArm], vec3(1, 0, 0));
    m = T * Rx;
    nodes[LeftUpperArm] = Node(m, left_upper_arm, &nodes[RightUpperArm], &nodes[LeftLowerArm]);

    T = translate(mat4(1.0), vec3(TORSO_WIDTH + UPPER_ARM_WIDTH, 0.9 * TORSO_HEIGHT, 0.0));
    Rx = rotate(mat4(1.0), DegreesToRadians * theta[RightUpperArm], vec3(1, 0, 0));
    m = T * Rx;
    nodes[RightUpperArm] = Node(m, right_upper_arm, &nodes[LeftUpperLeg], &nodes[RightLowerArm]);

    T = translate(mat4(1.0), vec3(-(TORSO_WIDTH + UPPER_LEG_WIDTH), 0.1 * UPPER_LEG_HEIGHT, 0.0));
    Rx = rotate(mat4(1.0), DegreesToRadians * theta[LeftUpperLeg], vec3(1, 0, 0));
    m = T * Rx;
    nodes[LeftUpperLeg] = Node(m, left_upper_leg, &nodes[RightUpperLeg], &nodes[LeftLowerLeg]);

    T = translate(mat4(1.0), vec3(TORSO_WIDTH + UPPER_LEG_WIDTH, 0.1 * UPPER_LEG_HEIGHT, 0.0));
    Rx = rotate(mat4(1.0), DegreesToRadians * theta[RightUpperLeg], vec3(1, 0, 0));
    m = T * Rx;
    nodes[RightUpperLeg] = Node(m, right_upper_leg, NULL, &nodes[RightLowerLeg]);

    T = translate(mat4(1.0), vec3(0.0, UPPER_ARM_HEIGHT, 0.0));
    Rx = rotate(mat4(1.0), DegreesToRadians * theta[LeftLowerArm], vec3(1, 0, 0));
    m = T * Rx;
    nodes[LeftLowerArm] = Node(m, left_lower_arm, NULL, NULL);

    T = translate(mat4(1.0), vec3(0.0, UPPER_ARM_HEIGHT, 0.0));
    Rx = rotate(mat4(1.0), DegreesToRadians * theta[RightLowerArm], vec3(1, 0, 0));
    m = T * Rx;
    nodes[RightLowerArm] = Node(m, right_lower_arm, NULL, NULL);

    T = translate(mat4(1.0), vec3(0.0, UPPER_LEG_HEIGHT, 0.0));
    Rx = rotate(mat4(1.0), DegreesToRadians * theta[LeftLowerLeg], vec3(1, 0, 0));
    m = T * Rx;
    nodes[LeftLowerLeg] = Node(m, left_lower_leg, NULL, NULL);

    T = translate(mat4(1.0), vec3(0.0, UPPER_LEG_HEIGHT, 0.0));
    Rx = rotate(mat4(1.0), DegreesToRadians * theta[RightLowerLeg], vec3(1, 0, 0));
    m = T * Rx;
    nodes[RightLowerLeg] = Node(m, right_lower_leg, NULL, NULL);
}

//----------------------------------------------------------------------------

void
init(void)
{
    colorcube();

    // Initialize tree
    initNodes();

    // Create a vertex array object
    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    // Create and initialize a buffer object
    GLuint buffer;
    glGenBuffers(1, &buffer);
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(points) + sizeof(colors),
        NULL, GL_DYNAMIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(points), points);
    glBufferSubData(GL_ARRAY_BUFFER, sizeof(points), sizeof(colors),
        colors);

    // Load shaders and use the resulting shader program
    GLuint program = InitShader("vshader.glsl", "fshader.glsl");
    glUseProgram(program);

    GLuint vPosition = glGetAttribLocation(program, "vPosition");
    glEnableVertexAttribArray(vPosition);
    glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0,
        BUFFER_OFFSET(0));

    GLuint vColor = glGetAttribLocation(program, "vColor");
    glEnableVertexAttribArray(vColor);
    glVertexAttribPointer(vColor, 4, GL_FLOAT, GL_FALSE, 0,
        BUFFER_OFFSET(points));

    ModelView = glGetUniformLocation(program, "ModelView");
    Projection = glGetUniformLocation(program, "Projection");

    glEnable(GL_DEPTH_TEST);
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    glClearColor(1.0, 1.0, 1.0, 1.0);

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

int
main(int argc, char** argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(512, 512);
    glutInitContextVersion(3, 2);
    glutInitContextProfile(GLUT_CORE_PROFILE);
    glutCreateWindow("robot");

    glewInit();

    init();

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    glutMouseFunc(mouse);

    glutCreateMenu(menu);
    glutAddMenuEntry("torso", Torso);
    glutAddMenuEntry("head1", Head1);
    glutAddMenuEntry("head2", Head2);
    glutAddMenuEntry("right_upper_arm", RightUpperArm);
    glutAddMenuEntry("right_lower_arm", RightLowerArm);
    glutAddMenuEntry("left_upper_arm", LeftUpperArm);
    glutAddMenuEntry("left_lower_arm", LeftLowerArm);
    glutAddMenuEntry("right_upper_leg", RightUpperLeg);
    glutAddMenuEntry("right_lower_leg", RightLowerLeg);
    glutAddMenuEntry("left_upper_leg", LeftUpperLeg);
    glutAddMenuEntry("left_lower_leg", LeftLowerLeg);
    glutAddMenuEntry("quit", Quit);
    glutAttachMenu(GLUT_MIDDLE_BUTTON);

    glutMainLoop();
    return 0;
}
