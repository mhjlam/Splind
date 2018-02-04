#include "wavy.h"

#include <sstream>
#include <glm/vec3.hpp>
#include <GL/freeglut.h>

wavy::wavy()
{
	acp = -1;
	mode = MOUSE_MODE_NONE;
	degree = DEGREE_QUADRATIC;
}

wavy::~wavy()
{
	clear_scene();
}

void wavy::add_cp(glm::vec2 p)
{
	// cannot exceed maximum number of allowed control points
	if (cps.size() == MAX_CPTS) return;

	// a point is already within this vicinity (so will be selected)
	if (select_cp(p)) return;

	// add control point
	acp = (int)cps.size();
	cps.push_back(glm::vec2(p));

	// recalculate the new B-Spline because a new control point has been added
	knot_vector(false);	// RECALCULATES THE KNOT VECTOR LOCALLY ONLY
	spline();			// Basis functions will be recalculated completely
}

void wavy::move_cp(glm::vec2 p)
{
	if (acp != -1 && acp < (int)cps.size())
	{
		cps[acp].x = p.x;
		cps[acp].y = p.y;
	}

	// the knot vector does not change when moving points (so recalculation is not required)
	spline();	// Basis functions will be recalculated completely
}

bool wavy::select_cp(glm::vec2 p)
{
	if (mode != MOUSE_MODE_NONE) return false;

	// deselect active cp
	acp = -1;

	// select control point closest to mouse pointer's location
	for (int i = 0; i < (int)cps.size(); ++i)
	{
		if ((abs((int)cps[i].x - p.x) < CP_RANGE && abs((int)cps[i].y - p.y) < CP_RANGE))
		{
			// set new active cp
			acp = i;
			return true;
		}
	}

	return false;
}

void wavy::mouse_click(int button, int state, int x, int y) // (x,y) coordinates of the mouse relative to the upper left corner
{
	if (state == GLUT_UP)
	{
		if (mode == MOUSE_MODE_DRAG && button == GLUT_LEFT_BUTTON) mode = MOUSE_MODE_NONE;
		else if (mode == MOUSE_MODE_NONE) add_cp(mpos);
	}
	else if (state == GLUT_DOWN)
	{
		if (button == GLUT_LEFT_BUTTON)
		{
			if (select_cp(glm::vec2(x, y)) && mode != MOUSE_MODE_MENU) mode = MOUSE_MODE_DRAG;
			else mode = MOUSE_MODE_NONE;

		}
		else if (button == GLUT_RIGHT_BUTTON)
		{
			mode = MOUSE_MODE_MENU;
		}
	}
}

void wavy::mouse_motion(int x, int y)
{
	// update mouse position
	mpos.x = (float)x;
	mpos.y = (float)y;

	// move active control point
	if (mode == MOUSE_MODE_DRAG) move_cp(glm::vec2(x, y));
}

void wavy::menu_select(int option)
{
	switch (option)
	{
		case MENU_CLEAR_ALL:
			clear_scene();
		break;

		case MENU_DEGREE: // switch degree
			(degree == DEGREE_QUADRATIC) ? degree = DEGREE_CUBIC : degree = DEGREE_QUADRATIC;

			// recalculate all knot values and basis functions (this is never possible with local propagation)
			knot_vector(true);
			spline();
		break;

		case MENU_CP_LINES:
			draw_cps = !draw_cps;
			break;
	}
}

void wavy::clear_scene()
{
	t.clear();		// clear knot vectors
	q.clear();		// clear segments
	cps.clear(); 	// clear control points
}

void wavy::draw()
{
	// draw control points
	if (draw_cps)
	{
		for (auto cp : cps)
		{
			glColor3f(0.0f, 0.0f, 0.0f);

			if (acp != -1 && acp < (int)cps.size() && cp == cps[acp])
				glColor3f(1.0f, 0.0f, 0.0f);

			glBegin(GL_QUADS);
			{
				glVertex2f(cp.x - (CPT_SIZE / 2), cp.y - (CPT_SIZE / 2));
				glVertex2f(cp.x + (CPT_SIZE / 2), cp.y - (CPT_SIZE / 2));
				glVertex2f(cp.x + (CPT_SIZE / 2), cp.y + (CPT_SIZE / 2));
				glVertex2f(cp.x - (CPT_SIZE / 2), cp.y + (CPT_SIZE / 2));
			}
			glEnd();
		}

		// draw lines between control points
		if (cps.size() > 1) // at least 2 points to draw a line between
		{
			glColor3f(0.0f, 0.0f, 0.0f);

			glBegin(GL_LINE_STRIP);
			{
				for (auto cp : cps) glVertex2f(cp.x, cp.y);
			}
			glEnd();
		}
	}

	// draw curve
	glLineWidth(2.0f);
	glColor3f(0.0f, 0.0f, 1.0f);

	glBegin(GL_LINE_STRIP);
	{
		for (auto seg : q) glVertex2f(seg.x, seg.y);
	}
	glEnd();
	glLineWidth(1.0f);
}

void wavy::draw_ui()
{
	std::stringstream ss;
	ss << "Coordinates: (" << mpos.x << ", " << mpos.y << ")";
	write_text(ss.str(), 1, 10);

	std::stringstream().swap(ss);
	ss << "Degree: ";
	degree == DEGREE_QUADRATIC ? ss << "quadratic" : ss << "cubic";
	write_text(ss.str(), 1, 25);
}

void wavy::write_text(std::string s, unsigned int x, unsigned int y)
{
	glColor3f(0.0f, 0.0f, 0.0f);
	glRasterPos2i(x, y);

	for (int i = 0; i < (int)s.length(); i++)
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_10, s[i]);
}

uint32_t wavy::knot_value(int j, int n, int d)
{
	int value = 0;

	if (j < d)				   value = 0;		// if (j < d) 				t[j] = 0;
	else if (d <= j && j <= n) value = j-d+1;	// if (d <= j && j <= n)	t[j] = j-d+1;
	else if (j > n)            value = n-d+2;	// if (j > n)				t[j] = n-d+2;

	return value;
}

void wavy::knot_vector(bool recalculate)
{
	// After adding the new knot value into the old knot vector, the parameters that needed to be computed are:
	// 1) Uniform: 					t[m-1]
	// 2) Non-uniform (quadratic): 	t[m-1], t[m-2], t[m-3]
	// 3) Non-uniform (cubic): 		t[m-1], t[m-2], t[m-3], t[m-4]

	int n = (int)cps.size()-1;		// number of control points - 1
	int d = degree+1;				// degree of the polynomial + 1
	unsigned int m = n + d + 1;

	if (n < d) return;

	int j = n - degree; // number of t values that need to be updated

	unsigned int start = j - 1 + d + 1;

	// initialize / recalculate all knot values (start at index 0)
	if (recalculate || n == d) start = 0;

	if (start > t.size()) start = (int)t.size();

	// knots between 'start' and 'm' will be updated, others require no updating
	for (unsigned int j = start; j < m; j++)
	{
		if (j < t.size()) t.at(j) = knot_value(j, n, d);
		else t.push_back(knot_value(j, n, d));
	}
}

float wavy::basis_function(int j, int d, float u)
{
	// Compute B[j,d](u) (all basis functions)

	if (d == 1)
	{
		//  When d = 1:
		// 	B[j,1](u) = (t[j] <= u < t[j+1]) ? 1 : 0

		if (t.at(j) <= u && u < t.at(j+1)) return 1.0f;
		else return 0.0f;
	}
	else // if (d > 1)
	{
		//  When d > 1:
		// 	B[j,d](u) = ((u - t[j])/(t[j+d-1] - t[j])) * B[j,d-1](u) + ((t[j+d] - u)/(t[j+d] - t[j+1])) * B[j+1,d-1](u)

		float div1 = 1.0f;
		float num1 = (u - t[j]);
		float denum1 = (float)(t[j+d-1] - t[j]);

		if (denum1 == 0.0f) div1 = 0.0f;		// define (u/0) as 0 (to avoid devision by zero)
		else div1 = num1 / denum1;

		float div2 = 1.0f;
		float num2 = (t[j+d] - u);
		float denum2 = (float)(t[j+d] - t[j+1]);

		if (denum2 == 0.0f) div2 = 0.0f;
		else div2 = num2 / denum2;

		// definition of B-Spline basis function
		return (div1 * basis_function(j, d-1, u) + div2 * basis_function(j+1, d-1, u));
	}
}

void wavy::spline()
{
	int n = (int)cps.size() - 1;	// one less than the number of control points
	int d = degree + 1;				// either three (quadratic) or four (cubic)
	if (n < d) return;				// the value of n must be equal or larger to d before a curve can be computed
	q.clear();						// remove all previous segments if we want to recalculate

	// variable 'u' must be between t[d-1] and t[n+1])
	for (float u = (float)t[d-1]; u <= (float)t[n+1]; u += U_INTERVAL)
	{
		// segment (x,y)
		glm::vec2 s(0,0);

		// definition of the B-Spline curve
		for (unsigned int j = 0; j < cps.size(); j++)
		{
			float b = basis_function(j, d, u);

			s.x += cps[j].x * b;
			s.y += cps[j].y * b;
		}

		// to ensure that no problems occur when drawing
		if (s.x != 0 && s.y != 0) q.push_back(s); // add new segment to curve that will be drawn
	}

	// add a final segment to the curve explicitly when it has just been intialized (this is no longer needed for new points)
	if (n == d) q.push_back(glm::vec2(cps.back().x, cps.back().y));
}
