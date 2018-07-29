#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
using namespace glm;

#include <collision/MinowskiPoint.h>


/*
 * Helper container class for the EPAPolytope that represents faces of the polytope
 */ 
class MinowskiTriangle
{
public:
	MinowskiPoint a;
	MinowskiPoint b;
	MinowskiPoint c;
	dvec3 normal;

	MinowskiTriangle() { }

	MinowskiTriangle(MinowskiPoint a, MinowskiPoint b, MinowskiPoint c) : a(a), b(b), c(c)
	{
		normal = normalize(cross((b.p-a.p), (c.p-a.p)));
	}

	dvec3 InterpolateContact()
	{
		// calculate barycentric coordinates of origin 
		// http://gamedev.stackexchange.com/questions/23743/whats-the-most-efficient-way-to-find-barycentric-coordinates
		dvec3 v0 = b.p - a.p;
		dvec3 v1 = c.p - a.p;
		dvec3 v2 = -a.p;

		double d00 = dot(v0, v0);
		double d01 = dot(v0, v1);
		double d11 = dot(v1, v1);
		double d20 = dot(v2, v0);
		double d21 = dot(v2, v1);
		double denom = d00 * d11 - d01 * d01;
		double v = (d11 * d20 - d01 * d21) / denom;
		double w = (d00 * d21 - d01 * d20) / denom;
		double u = 1.0f - v - w;

		return u*a.support + v*b.support + w*c.support;
	}
};
