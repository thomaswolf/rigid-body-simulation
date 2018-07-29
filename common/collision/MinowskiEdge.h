#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
using namespace glm;

/*
 * Helper container class for the EPAPolytope
 */
class MinowskiEdge
{
public:
	MinowskiPoint a;
	MinowskiPoint b;

	MinowskiEdge(MinowskiPoint a, MinowskiPoint b) : a(a), b(b)
	{
	}
};
