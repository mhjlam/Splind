#pragma once

#include "constants.h"

#include <string>
#include <vector>
#include <glm/vec2.hpp>

class wavy
{
private:
	int acp;						// active control point (an index of cps)
	std::vector<glm::vec2> cps;		// collection of control points

	glm::vec2 mpos;					// mouse coordinates
	std::vector<int> t;				// knot vector
	std::vector<glm::vec2> q;		// segments of the curve

public:
	enum MouseMode mode;
	enum PolyDegree degree;
	bool draw_cps = true;

public:
	wavy();
	~wavy();

	void add_cp(glm::vec2 p);		// add new control point at given x and y
	void move_cp(glm::vec2 p);		// move active control point to new position
	bool select_cp(glm::vec2 p);

	void mouse_click(int button, int state, int x, int y);
	void mouse_motion(int x, int y);
	void menu_select(int option);

	void draw();
	void clear_scene();

	void draw_ui();
	void write_text(std::string s, unsigned int x, unsigned int y);

	uint32_t knot_value(int j, int n, int d);
	void knot_vector(bool recalculate = false);

	float basis_function(int j, int d, float u);
	void spline();
};
