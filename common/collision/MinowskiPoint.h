#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
using namespace glm;

/*
 * Container class to represent points in minowski space with the shape support from the world space
 */
class MinowskiPoint
{
public:
	dvec3 p;	 // point in minowski space
	dvec3 support; // original point of the shape of the rigidbody in world coordinates

	MinowskiPoint() {}
	MinowskiPoint(dvec3 p, dvec3 support) : p(p), support(support)
	{
	}

	bool operator==(const MinowskiPoint& other) const { return p == other.p; }
};

