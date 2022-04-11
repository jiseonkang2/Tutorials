#include <iostream>
#include <GL/glut.h>  
#include <Math.h> 

#define PI 3.14159265f

GLint windowWidth = 512;
GLint windowHeight = 512;

GLfloat ballRadius = 0.5f;   // Radius of the ball
GLfloat ballX = 0.0f;        // Center position of the ball
GLfloat ballY = 0.0f;
GLfloat colors[][3] = {
    { 1.0, 0.0, 0.0 }, // red
    { 0.0, 1.0, 0.0 }, // green
    { 0.0, 0.0, 1.0 }  // blue
};
int colorIdx = 0;

enum actions { NONE, MOVE_OBJECT };
int action = NONE;

void display()
{
    glClear(GL_COLOR_BUFFER_BIT);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glTranslatef(ballX, ballY, 0.0f);
    glBegin(GL_TRIANGLE_FAN);
    glColor3fv(colors[colorIdx]);
    glVertex2f(0.0f, 0.0f);       // Center of circle
    int numSegments = 100;
    GLfloat angle;
    for (int i = 0; i <= numSegments; i++) {
        angle = i * 2.0f * PI / numSegments;
        glVertex2f(cos(angle) * ballRadius, sin(angle) * ballRadius);
    }
    glEnd();

    glutSwapBuffers();
}

void keyboard(unsigned char key, int x, int y)
{
    switch (key)
    {
    case 27:     // ESC key
        exit(0);
        break;
    }
}

void special(int key, int x, int y)
{
    switch (key) {
    case GLUT_KEY_F1:
        colorIdx = (colorIdx + 1) % 3;
        break;
    }

    glutPostRedisplay();
}

void mouse(int button, int state, int x, int y)
{
    if (button == GLUT_LEFT_BUTTON)
    {
        if (state == GLUT_DOWN)
        {
            action = MOVE_OBJECT;

            float xx = 2.0 * x / (float)windowWidth - 1.0;
            float yy = 1.0 - 2.0 * y / (float)windowHeight;
            ballX = xx;
            ballY = yy;
            printf("(%d, %d) -> (%f, %f)\n", x, y, xx, yy);
        }
        else // GLUT_UP
        {
            action = NONE;
        }
    }

    glutPostRedisplay();
}

void motion(int x, int y)
{
    if (action == MOVE_OBJECT)
    {
        float xx = 2.0 * x / (float)windowWidth - 1.0;
        float yy = 1.0 - 2.0 * y / (float)windowHeight;
        ballX = xx;
        ballY = yy;
        printf("(%d, %d) -> (%f, %f)\n", x, y, xx, yy);
    }

    glutPostRedisplay();
}

int main(int argc, char** argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(windowWidth, windowHeight);
    glutInitWindowPosition(0, 0);
    glutCreateWindow("Handling Ball");

    glutDisplayFunc(display);
    glutKeyboardFunc(keyboard);
    glutSpecialFunc(special);
    glutMouseFunc(mouse);
    glutMotionFunc(motion);

    glutMainLoop();

    return EXIT_SUCCESS;
}
