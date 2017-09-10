/*
 * SPLINER
 *
 * Author	Maurits Lam
 * Date		September 2017
 *
 * Description
 * 	Quadratic and cubic uniform and non-uniform B-splines.
 *	Uses fixed function pipeline OpenGL.
 */

#include "spliner.h"
#include <GL/freeglut.h>

spliner app = spliner();

void display()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	app.draw();
	app.draw_ui();

	glutSwapBuffers();
}

void reshape(int w, int h)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glViewport(0, 0, w, h);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0, w, h, 0); // left, right, bottom, top
	glMatrixMode(GL_MODELVIEW);
}

void click(int button, int state, int x, int y)
{
	app.mouse_click(button, state, x, y);
	glutPostRedisplay();
}

void motion(int x, int y)
{
	app.mouse_motion(x, y);
	glutPostRedisplay();
}

void menu(int option)
{
	app.menu_select(option);
}


int main(int argc, char* argv[])
{
	// initialization
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glutInitWindowSize(WIDTH, HEIGHT);
	glutInitWindowPosition(100, 100);
	glutCreateWindow("curvus");
	glDisable(GL_DEPTH_TEST);

	// background color
	glClearColor(0.85f, 0.85f, 0.80f, 1.0f);

	// call-back procedures
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);

	// mouse input
	glutMouseFunc(click);
	glutMotionFunc(motion);
	glutPassiveMotionFunc(motion);

	// pop-up menu
	glutCreateMenu(menu);
	glutAddMenuEntry("Clear control points", MENU_CLEAR_ALL);
	glutAddMenuEntry("Toggle degree", MENU_DEGREE);
	glutAddMenuEntry("Toggle uniformity", MENU_UNIFORMITY);
	glutAddMenuEntry("Toggle control point lines", MENU_CP_LINES);
	glutAttachMenu(GLUT_RIGHT_BUTTON);

	// start loop
	glutMainLoop();

	return 0;
}

