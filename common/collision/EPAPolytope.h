#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
using namespace glm;

#include <list>
#include <limits>

#include <collision/MinowskiPoint.h>
#include <collision/MinowskiTriangle.h>
#include <collision/MinowskiEdge.h>


/* 
 * Simplex helper class of polytopes for the EPA algorithm
 * Processing similar to: http://hacktank.net/blog/?p=93
 */
class EPAPolytope
{
private:
	// triangles that describes our polytope
	std::list<MinowskiTriangle> triangles;
	std::list<MinowskiEdge> edges;  // keeps track of edges we use for creating triangles when adding a new point

public:
	void AddTriangle(MinowskiPoint a, MinowskiPoint b, MinowskiPoint c)
	{
		triangles.push_back(MinowskiTriangle(a,b,c));
	}

	void ClosestFaceToOrigin(double& shortestDst, MinowskiTriangle** closestFace)
	{
		shortestDst = FLT_MAX;
		
		auto i = triangles.begin();
		while (i != triangles.end())
		{
			double dst = dot(i->normal, i->a.p);

			if (std::abs(dst) < std::abs(shortestDst))
			{
				shortestDst = dst;
				*closestFace = &(*i); // avoid copying
			}

			++i;
		}
	}

	void AddPoint(MinowskiPoint m)
	{
		auto i = triangles.begin();
		while (i != triangles.end())
		{
			if (dot(i->normal, (m.p - i->a.p)) > 0)
			{
				AddEdge(i->a,i->b);
				AddEdge(i->b,i->c);
				AddEdge(i->c,i->a);

				// remove triangle	
				i = triangles.erase(i);
			}
			else ++i;
		}
	
		// fill in triangles
		for (MinowskiEdge& e : edges)
		{
			triangles.push_back(MinowskiTriangle(m, e.a, e.b));
		}
		edges.clear();
	}

private:
	void AddEdge(MinowskiPoint a, MinowskiPoint b)
	{
		auto i = edges.begin();
		while (i != edges.end())
		{
			if (i->a == b && i->b == a)
			{
				// remove opposite edge	
				i = edges.erase(i);
				return;
			}
			else ++i;
		}
		edges.push_back(MinowskiEdge(a,b));
	}
};
