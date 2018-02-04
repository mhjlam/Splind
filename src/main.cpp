#include <string>
#include <GL/freeglut.h>

#include "wavy.h"

void display()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	wavy* w = (wavy*)glutGetWindowData();
	w->draw();
	w->draw_ui();

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
	wavy* w = (wavy*)glutGetWindowData();
	w->mouse_click(button, state, x, y);
	glutPostRedisplay();
}

void motion(int x, int y)
{
	wavy* w = (wavy*)glutGetWindowData();
	w->mouse_motion(x, y);
	glutPostRedisplay();
}

void keyboard(unsigned char key, int x, int y)
{
	if (key == 27)
	{
	 	glutLeaveMainLoop();
	}
}

void menu(int option)
{
	wavy* w = (wavy*)glutGetWindowData();
	w->menu_select(option);
}

int main(int argc, char* argv[])
{
	// initialization
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);

	int window_center_x = (glutGet(GLUT_SCREEN_WIDTH) - WINDOW_WIDTH) / 2.0f;
	int window_center_y = (glutGet(GLUT_SCREEN_HEIGHT) - WINDOW_HEIGHT) / 2.0f;

	glutInitWindowPosition(window_center_x, window_center_y);
	glutCreateWindow("wavy");

	// setup opengl
	glDisable(GL_DEPTH_TEST);
	glClearColor(0.85f, 0.85f, 0.80f, 1.0f);

	// initialize application
	wavy w = wavy();
	glutSetWindowData(&w);

	// draw
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);

	// input
	glutMouseFunc(click);
	glutMotionFunc(motion);
	glutPassiveMotionFunc(motion);
	glutKeyboardFunc(keyboard);

	// attach pop-up menu to right mouse button
	glutCreateMenu(menu);
	glutAddMenuEntry("Clear", MENU_CLEAR_ALL);
	glutAddMenuEntry("Degree: Quadratic | Cubic", MENU_DEGREE);
	glutAddMenuEntry("Control points: Show | Hide", MENU_CP_LINES);
	glutAttachMenu(GLUT_RIGHT_BUTTON);

	// start loop
	glutMainLoop();

	return 0;
}
