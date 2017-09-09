#pragma once

#include <stdint.h>

static const uint32_t WIDTH = 1280;
static const uint32_t HEIGHT = 720;
static const uint32_t MAX_CPTS = 10;	// maximum number of controls points
static const uint32_t CP_RANGE = 15;	// how many pixels the mouse pointer can be off for the point to be selected
static const uint32_t CPT_SIZE = 6;		// size of the point in pixels (number must be even)
static const float U_INTERVAL = 0.1f;	// curve precision


enum Uniformity
{
	UNIFORM,
	NON_UNIFORM
};

enum MouseMode
{
	MOUSE_MODE_NONE,
	MOUSE_MODE_DRAG,
	MOUSE_MODE_MENU
};

enum PolyDegree // degree of the polynomial
{
	DEGREE_QUADRATIC = 2,
	DEGREE_CUBIC = 3
};

enum MenuOptions
{
	MENU_CLEAR_ALL,
	MENU_DEGREE,
	MENU_UNIFORMITY,
	MENU_CP_LINES
};
