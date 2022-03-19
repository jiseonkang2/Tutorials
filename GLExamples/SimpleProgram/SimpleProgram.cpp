// SimpleProgram.cpp: A Simple Program
//

#include <iostream>

#include <GL/glut.h>


/* Display callback function */
void mydisplay()
{
	glClear(GL_COLOR_BUFFER_BIT);

	glBegin(GL_QUADS);
	// red
	glColor3f(1.0, 0., 0.);
	glVertex2f(-0.5, -0.5);
	// green
	glColor3f(0., 1., 0.);
	glVertex2f(-0.5, 0.5);
	// blue
	glColor3f(0., 0., 1.);
	glVertex2f(0.5, 0.5);
	// black
	glColor3f(0., 0., 0.);
	glVertex2f(0.5, -0.5);
	glEnd();

	glFlush();
}

int main(int argc, char** argv)
{
	glutInit(&argc, argv);

	glutCreateWindow("simple");
	glutDisplayFunc(mydisplay);

	glutMainLoop();

	return EXIT_SUCCESS;
}
