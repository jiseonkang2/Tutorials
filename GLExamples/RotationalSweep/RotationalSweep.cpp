// RotationalSweep.cpp : 이 파일에는 'main' 함수가 포함됩니다. 거기서 프로그램 실행이 시작되고 종료됩니다.
//

#pragma warning(disable:4996)

#include <stdio.h>
#include <stdlib.h>
#include <Math.h>     // Needed for sin, cos
#include <GL/glut.h>
#include <glm/glm.hpp>

#include "ControlPoint.h"

#define M_PI 3.14159265f
#define POINT_BOUNDARY  0.01
#define RADIUS_SWEEP	1.0

using namespace glm;

typedef enum
{
	NONE,
	CATMULLROM,
	BSPLINE,
	SUBBSPLINE,
	INTERPOLATSUB
} CURVETYPE;
CURVETYPE ctype = NONE;

static int width = 500, height = 500; // window width and height

CPLIST* cplist;
int		cpindex;
bool	cpmoveflag = false;
bool	closeflag = false;
bool	mouseRotateFlag = false;
vec3	normal;		// 마우스 회전 축
float	theta = 0;	// 마우스 회전 각
vec3	mousepos;			// 마우스 포인트

float color[][3] = {
	{0.0, 0.0, 0.0}, // Black
	{1.0, 0.0, 0.0}, // R
	{0.0, 1.0, 0.0}, // G
	{0.0, 0.0, 1.0}, // B
	{1.0, 1.0, 0.0},
	{0.0, 1.0, 1.0},
	{1.0, 0.0, 1.0},
	{0.1, 1.0, 1.0} //White
};

static GLfloat M0[4][4] = {
	{1.0, 0.0, 0.0, 0.0},
	{0.0, 1.0, 0.0, 0.0},
	{0.0, 0.0, 1.0, 0.0},
	{0.0, 0.0, 0.0, 1.0}
};
static GLfloat T[4][4] = {
	{1.0, 0.0, 0.0, 0.0},
	{0.0, 1.0, 0.0, 0.0},
	{0.0, 0.0, 1.0, 0.0},
	{0.0, 0.0, 0.0, 1.0}
};

float M[][4] =
{
	{ 1.0, 0.0, 0.0, 0.0},
	{ 0.0, 1.0, 0.0, 0.0},
	{ 0.0, 0.0, 1.0, 0.0},
	{ 0.0, 0.0, 0.0, 1.0}
};

float CatmullRom[][4] =
{
	{0.0, 1.0, 0.0, 0.0},
	{-0.5, 0.0, 0.5, 0.0},
	{1.0, -2.5, 2.0, -0.5},
	{-0.5, 1.5, -1.5, 0.5}
};

float BSpline[][4] =
{
	{1.0 / 6.0, 4.0 / 6.0, 1.0 / 6.0, 0.0},
	{-3.0 / 6.0, 0.0, 3.0 / 6.0, 0.0},
	{3.0 / 6.0, -6.0 / 6.0, 3.0 / 6.0, 0.0},
	{-1.0 / 6.0, 3.0 / 6.0, -3.0 / 6.0, 1.0 / 6.0}
};

float SubBSpline[][4] =
{
	{4.0 / 8.0, 4.0 / 8.0, 0.0, 0.0},
	{1.0 / 8.0, 6.0 / 8.0, 1.0 / 8.0, 0.0},
	{0.0, 4.0 / 8.0, 4.0 / 8.0, 0.0},
	{0.0, 1.0 / 8.0, 6.0 / 8.0, 1.0 / 8.0},
	{0.0, 0.0, 4.0 / 8.0, 4.0 / 8.0},
};

float InterSub[][4] =
{
	{-1.0 / 16.0, 9.0 / 16.0, 9.0 / 16.0, -1.0 / 16.0}
};

void keyboard(unsigned char key, int x, int y);
void changeOrtho();
void initlights(void);
void init();
void setMatrix(CURVETYPE type);
void calcVertex(float* v, float t, GLfloat* p0, GLfloat* p1, GLfloat* p2, GLfloat* p3);
void drawCurve();
void calcBSplineVertex(GLfloat v[5][3], GLfloat p[4][3]);
void calcInterVertex(GLfloat v[2][3], GLfloat* p0, GLfloat* p1, GLfloat* p2, GLfloat* p3);
void Subdivision();
vec3 GetTrackballPos(int x, int y);


void initlights(void)
{
	GLfloat ambient[] = { 0.2, 0.2, 0.2, 1.0 };
	GLfloat position[] = { 1.0, 0.0, 2.0, 1.0 };
	GLfloat mat_diffuse[] = { 0.6, 0.6, 0.6, 1.0 };
	GLfloat mat_specular[] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat mat_shininess[] = { 50.0 };

	//glEnable(GL_LIGHTING);
	//glEnable(GL_LIGHT0);

	glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
	glLightfv(GL_LIGHT0, GL_POSITION, position);

	glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
	glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);
}

void init()
{
	if (cplist != NULL)
		CP_deleteall(cplist);
	else
		cplist = (CPLIST*)malloc(sizeof(CPLIST));
	CP_init(cplist);

	cpmoveflag = false;
	closeflag = false;
	theta = 0;

	glClearColor(1.0, 1.0, 1.0, 1.0);
	glColor3f(0.0, 0.0, 0.0);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_AUTO_NORMAL);

	initlights();

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-1.0, 1.0, -1.0, 1.0, -1.0, 1.0);

	glPolygonMode(GL_FRONT, GL_FILL);
	glPolygonMode(GL_BACK, GL_FILL);
	glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
	glEnable(GL_COLOR_MATERIAL);
	glShadeModel(GL_FLAT);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void setMatrix(CURVETYPE type)
{
	int i, j;
	switch (type) {
	case CATMULLROM:
		for (i = 0; i < 4; i++)
			for (j = 0; j < 4; j++)
				M[i][j] = CatmullRom[i][j];
		break;
	case BSPLINE:
		for (i = 0; i < 4; i++)
			for (j = 0; j < 4; j++)
				M[i][j] = BSpline[i][j];
		break;
	}
}

void calcVertex(float* v, float t, GLfloat* p0, GLfloat* p1, GLfloat* p2, GLfloat* p3)
{
	double c1, c2, c3, c4;

	for (int i = 0; i < 2; i++)
	{
		c1 = M[0][0] * p0[i] + M[0][1] * p1[i] + M[0][2] * p2[i] + M[0][3] * p3[i];
		c2 = M[1][0] * p0[i] + M[1][1] * p1[i] + M[1][2] * p2[i] + M[1][3] * p3[i];
		c3 = M[2][0] * p0[i] + M[2][1] * p1[i] + M[2][2] * p2[i] + M[2][3] * p3[i];
		c4 = M[3][0] * p0[i] + M[3][1] * p1[i] + M[3][2] * p2[i] + M[3][3] * p3[i];
		v[i] = c1 + t * (c2 + t * (c3 + t * c4));
	}
	v[2] = 0.0;
}

void drawCurve()
{
	glDisable(GL_LIGHTING);
	glDisable(GL_LIGHT0);

	if (ctype == NONE)
		return;

	setMatrix(ctype);

	GLfloat cp[50][3];
	CP_getArray(cplist, cp);

	if (ctype == CATMULLROM)
		glColor3fv(color[1]);
	else
		glColor3fv(color[2]);

	glBegin(GL_LINES);
	int interval = 10;
	GLfloat v[3];
	int max;
	if (closeflag)
		max = cplist->count + 1;
	else
		max = cplist->count - 2;

	for (int i = 1; i < max; i++)
	{
		for (int k = 1; k <= interval; k++)
		{
			calcVertex(v, (float)k / interval, cp[i - 1], cp[i], cp[i + 1], cp[i + 2]);
			glVertex3fv(v);
		}
	}
	glEnd();
}

void calcBSplineVertex(GLfloat v[5][3], GLfloat p[4][3])
{
	for (int i = 0; i < 5; i++)
		for (int j = 0; j < 3; j++)
		{
			v[i][j] = 0;
			for (int k = 0; k < 4; k++)
				v[i][j] += SubBSpline[i][k] * p[k][j];
		}
}

void calcInterVertex(GLfloat v[2][3], GLfloat* p0, GLfloat* p1, GLfloat* p2, GLfloat* p3)
{
	for (int i = 0; i < 3; i++) {
		v[0][i] = p1[i];
		v[1][i] = InterSub[0][0] * p0[i] + InterSub[0][1] * p1[i] + InterSub[0][2] * p2[i] + InterSub[0][3] * p3[i];
	}

}

void Subdivision()
{
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);

	GLfloat newP[200][3];
	GLfloat P[200][3];
	CP_getArray(cplist, P);
	int numP = cplist->count;
	int numSub = 3;

	// Subdivision 
	if (ctype == INTERPOLATSUB)
	{
		glColor3fv(color[3]);
		for (int j = 0; j < numSub; j++) {
			newP[0][0] = P[0][0];
			newP[0][1] = P[0][1];
			newP[0][2] = P[0][2];
			calcInterVertex(&newP[0], P[numP], P[0], P[1], P[2]);
			for (int i = 1; i < numP; i++)
				calcInterVertex(&newP[i * 2], P[i - 1], P[i], P[i + 1], P[i + 2]);
			numP = numP * 2;
			memcpy(&P[0][0], &newP[0][0], sizeof(GLfloat) * 3 * 100);

			for (int i = 0; i < 3; i++)
			{
				P[numP + i][0] = newP[i][0];
				P[numP + i][1] = newP[i][1];
				P[numP + i][2] = newP[i][2];
			}
		}
	}
	else //if(ctype == SUBBSPLINE)
	{
		glColor3fv(color[5]);
		for (int j = 0; j < numSub; j++) {
			for (int i = 0; i < numP; i++)
				calcBSplineVertex(&newP[i * 2], &P[i]);
			numP = numP * 2;
			memcpy(&P[0][0], &newP[0][0], sizeof(GLfloat) * 3 * 100);
		}
	}

	// Translation for sweep
	for (int j = 0; j < 100; j++)
		P[j][0] += RADIUS_SWEEP;

	// Sweeping
	float x1, y1, z1;
	float x2, y2, z2;
	float x3, y3, z3;
	float x4, y4, z4;
	int ntheta = 50;			//angular slices
	float dtheta = 2 * M_PI / ntheta;

	for (int i = 0; i < numP; i++) {
		x1 = P[i][0];
		y1 = P[i][1];
		z1 = P[i][2];
		x2 = P[i + 1][0];
		y2 = P[i + 1][1];
		z2 = P[i + 1][2];

		glVertex3f(x1, y1, z1);
		glVertex3f(x2, y2, z2);
		glBegin(GL_QUAD_STRIP);
		for (int j = 0; j <= ntheta; ++j) {
			theta += dtheta;
			double cosa = cos(theta);
			double sina = sin(theta);

			x3 = x1 * cosa - y1 * sina;
			y3 = y1;
			z3 = x1 * sina + z1 * cosa;

			x4 = x2 * cosa - y2 * sina;
			y4 = y2;
			z4 = x2 * sina + z2 * cosa;

			glVertex3f(x3, y3, z3);
			glVertex3f(x4, y4, z4);

		}
		glEnd();
	}
}

static void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);

	glPushMatrix();
	if (cplist->count >= 4)
	{
		switch (ctype) {
		case SUBBSPLINE:
		case INTERPOLATSUB:
			Subdivision();
			break;
		default:
			drawCurve();
			break;
		}
	}

	if (ctype != SUBBSPLINE && ctype != INTERPOLATSUB)
	{
		/* control point */
		glPointSize(5.0);
		glColor3f(0.0, 0.0, 0.0);
		glBegin(GL_POINTS);
		for (int i = 1; i <= (cplist->count); i++)
			glVertex3fv(CP_get(cplist, i));
		glEnd();
	}
	glPopMatrix();

	glFlush();
}

static void mouse(int button, int state, int x, int y)
{
	float wx, wy;

	if (button == GLUT_LEFT_BUTTON)
	{
		switch (state) {
		case GLUT_DOWN:
			wx = (2.0 * x) / (float)(width - 1) - 1.0;
			wy = (2.0 * (height - 1 - y)) / (float)(height - 1) - 1.0;

			GLfloat pos[3];
			pos[0] = wx;
			pos[1] = wy;
			pos[2] = 0.0;
			cpindex = CP_search(cplist, pos, POINT_BOUNDARY);

			if (glutGetModifiers() == GLUT_ACTIVE_CTRL && cpindex == 0)
				CP_insert(cplist, pos, cplist->count + 1);

			if (glutGetModifiers() == GLUT_ACTIVE_SHIFT)
				CP_delete(cplist, cpindex);

			if (glutGetModifiers() == GLUT_ACTIVE_ALT)
				cpmoveflag = true;
			break;
		case GLUT_UP:
			cpmoveflag = false;
			theta = 0.0;
			break;
		}
	}

	if (button == GLUT_RIGHT_BUTTON)
	{
		switch (state) {
		case GLUT_DOWN:
			mouseRotateFlag = true;
			mousepos.x = (float)x;
			mousepos.y = (float)y;
			break;
		case GLUT_UP:
			mouseRotateFlag = false;
			theta = 0.0;
			break;
		}
	}

	glutPostRedisplay();
}

vec3 GetTrackballPos(int x, int y)
{
	vec3 pos;
	float d;

	pos.x = (2.0 * x - width) / width;
	pos.y = (height - 2.0 * y) / height;
	pos.z = 0;

	d = pos.length();
	d = (d < 1.0) ? d : 1.0;
	pos.z = sqrtf(1.0 - d * d);
	
	pos = normalize(pos);

	return pos;
}

void motion(int x, int y)
{
	if (cpmoveflag)
	{
		float wx, wy;
		wx = (2.0 * x) / (float)(width - 1) - 1.0;
		wy = (2.0 * (height - 1 - y)) / (float)(height - 1) - 1.0;

		GLfloat pos[3];
		pos[0] = wx;
		pos[1] = wy;
		pos[2] = 0.0;

		if (cpindex != 0)
			CP_modify(cplist, pos, cpindex);

		glutPostRedisplay();
	}

	if (mouseRotateFlag &&
		(ctype == SUBBSPLINE || ctype == INTERPOLATSUB))
	{
		vec3 P1 = GetTrackballPos(mousepos.x, mousepos.y);
		vec3 P2 = GetTrackballPos(x, y);
		vec3 diffv = P2 - P1;
		theta = 90.0 * diffv.length();
		normal = P1 * P2;
		mousepos.x = x;
		mousepos.y = y;

		glMatrixMode(GL_MODELVIEW);
		glGetFloatv(GL_MODELVIEW_MATRIX, (GLfloat*)M0);
		glLoadIdentity();
		glRotatef(theta, normal.x, normal.y, normal.z);
		glMultMatrixf((GLfloat*)M0);

		glutPostRedisplay();
	}

	char info[128];
	sprintf(info, "(%d,%d)", x, y);
	glutSetWindowTitle(info);
}

void keyboard(unsigned char key, int x, int y)
{
	switch (key)
	{
	case 'q': case 'Q':
		exit(0);
		break;
	case '1':
		ctype = CATMULLROM;
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		glutPostRedisplay();
		break;
	case '2':
		ctype = BSPLINE;
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		glutPostRedisplay();
		break;
	case '3':
		ctype = INTERPOLATSUB;
		changeOrtho();
		glutPostRedisplay();
		break;
	case '4':
		ctype = SUBBSPLINE;
		changeOrtho();
		glutPostRedisplay();
		break;
	case 'p': case 'P':
		CP_print(cplist);
		break;
	}
}

void Special(int key, int, int) {
	switch (key) {
	case GLUT_KEY_HOME:
		ctype = NONE;
		closeflag = false;
		CP_deleteall(cplist);
		init();
		glutPostRedisplay();
		break;
	case GLUT_KEY_END:
		closeflag = true;
		glutPostRedisplay();
		break;
	default:
		return;
	}

	glutPostRedisplay();
}

void changeOrtho()
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	float x, max = 1.0;
	for (int i = 1; i <= cplist->count; i++) {
		GLfloat* p = CP_get(cplist, i);
		if (p == NULL)
			break;
		x = (abs((float)p[0]) + RADIUS_SWEEP > abs((float)p[1])) ? abs((float)p[0]) + RADIUS_SWEEP : abs((float)p[1]);
		max = (abs(x) > max) ? abs(x) : max;
	}

	glOrtho(-max, max, -max, max, -max, max);
}

void reshape(int w, int h)
{
	width = w;
	height = h;
	glViewport(0, 0, w, h);

	changeOrtho();
	//glMatrixMode(GL_PROJECTION);
	//glLoadIdentity();
	//
	//if(ctype == SUBBSPLINE || ctype == INTERPOLATSUB){
	//	float x, max = 1.0;
	//	for(int i = 0; i < cplist->count ; i++){
	//		GLfloat* p = CP_get(cplist, i);
	//		x = ((float)p[0]+RADIUS_SWEEP > (float)p[1]) ? (float)p[0]+RADIUS_SWEEP : (float)p[1];
	//		max = (abs(x) > max ) ? abs(x) : max;
	//	}
	//	glOrtho(-max, max, -max, max, -max, max);	
	//}
	//else
	//	glOrtho(-1.0, 1.0, -1.0, 1.0, -1.0, 1.0);	

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void printCommend()
{
	printf("::::::::::::::Rotational Sweep::::::::::::::\n\n");

	printf("Mouse control>>\n");
	printf(" LEFT_BUTTON_DOWN\n");
	printf("\t+ Ctrl - Add Control point\n");
	printf("\t+ Alt - Move Control point\n");
	printf("\t+ Shift - Delete Control point\n");
	printf(" RIGHT_BUTTON_DOWN\n");
	printf("\t+ Drag - Rotate the View\n\n");

	printf("Keyboard commands>>\n");
	printf(" HOME - Reset rotation\n");
	printf(" END - Close curves (Only for Camull-Rom & B-Spline)\n\n");

	printf(" 1 - Draw Camull-Rom curves\n");
	printf(" 2 - Draw B-Spline curves\n");
	printf(" 3 - Draw Interpolating subdivision\n");
	printf(" 4 - Draw Uniform cubic B-Spline subdivision\n\n");

	printf(" p - print control points\n");
	printf(" q - Quit\n\n");

}

int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(width, height);
	glutInitWindowPosition(100, 100);
	glutCreateWindow("Curves");

	glutDisplayFunc(display);
	glutMouseFunc(mouse);
	glutMotionFunc(motion);
	glutKeyboardFunc(keyboard);
	glutSpecialFunc(Special);
	glutReshapeFunc(reshape);

	init();
	printCommend();
	glutMainLoop();
}
